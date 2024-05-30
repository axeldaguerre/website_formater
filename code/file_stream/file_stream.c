////////////////////////////////
//~ rjf: root_globals

#if BUILD_CORE
FS_Shared *fs_shared = 0;
#endif

////////////////////////////////
//~ rjf: Basic Helpers

internal U64
FS_HashFromString(String8 string)
{
 U64 result = 5381;
 for(U64 i = 0; i < string.size; i += 1)
 {
  result = ((result << 5) + result) + string.str[i];
 }
 return result;
}

////////////////////////////////
//~ rjf: Top-Level API

internal FS_InitReceipt
FS_Init(OS_InitReceipt os_init, C_InitReceipt c_init)
{
 if(IsMainThread() && fs_shared == 0)
 {
  Arena *arena = ArenaAlloc(Gigabytes(8));
  fs_shared = PushArray(arena, FS_Shared, 1);
  fs_shared->arena = arena;
  fs_shared->u2s_ring_size  = Kilobytes(64);
  fs_shared->u2s_ring_base  = PushArrayNoZero(arena, U8, fs_shared->u2s_ring_size);
  fs_shared->u2s_ring_mutex = OS_MutexAlloc();
  fs_shared->u2s_ring_cv    = OS_ConditionVariableAlloc();
  fs_shared->slots_count = 1024;
  fs_shared->stripes_count = 64;
  fs_shared->slots = PushArray(fs_shared->arena, FS_Slot, fs_shared->slots_count);
  fs_shared->stripes = PushArray(fs_shared->arena, FS_Stripe, fs_shared->stripes_count);
  for(U64 idx = 0; idx < fs_shared->stripes_count; idx += 1)
  {
   fs_shared->stripes[idx].arena = ArenaAlloc(Gigabytes(8));
   fs_shared->stripes[idx].rw_mutex = OS_SRWMutexAlloc();
   fs_shared->stripes[idx].cv = OS_ConditionVariableAlloc();
  }
  fs_shared->stream_thread_count = Max(2, OS_LogicalProcessorCount()-1);
  fs_shared->stream_threads = PushArray(arena, OS_Handle, fs_shared->stream_thread_count);
  for(U64 thread_idx = 0;
      thread_idx < fs_shared->stream_thread_count;
      thread_idx += 1)
  {
   fs_shared->stream_threads[thread_idx] = OS_ThreadStart((void *)thread_idx, FS_StreamThreadEntryPoint);
  }
  fs_shared->scanner_thread = OS_ThreadStart(0, FS_ScannerThreadEntryPoint);
 }
 FS_InitReceipt result = {0};
 return result;
}

////////////////////////////////
//~ rjf: Cache Lookups

internal FS_Val
FS_ValFromPath(String8 path, U64 endt_us)
{
 FS_Val val = {0};
 U64 hash = FS_HashFromString(path);
 U64 slot_idx = hash%fs_shared->slots_count;
 U64 stripe_idx = slot_idx%fs_shared->stripes_count;
 FS_Slot *slot = &fs_shared->slots[slot_idx];
 FS_Stripe *stripe = &fs_shared->stripes[stripe_idx];
 B32 sent = 0;
 OS_SRWMutexScope_R(stripe->rw_mutex) for(B32 good = 0; !good;)
 {
  for(FS_Node *n = slot->first; n != 0; n = n->hash_next)
  {
   if(Str8Match(n->path, path, 0))
   {
    val = n->val;
    good = 1;
    break;
   }
  }
  if(!good && !sent)
  {
   OS_SRWMutexScopeLeave_R(stripe->rw_mutex);
   OS_SRWMutexScope_W(stripe->rw_mutex)
   {
    B32 node_found = 0;
    for(FS_Node *n = slot->first; n != 0; n = n->hash_next)
    {
     if(Str8Match(n->path, path, 0))
     {
      node_found = 1;
      break;
     }
    }
    if(!node_found)
    {
     FS_Node *node = PushArray(stripe->arena, FS_Node, 1);
     node->path = PushStr8Copy(stripe->arena, path);
     QueuePush_NZ(slot->first, slot->last, node, hash_next, CheckNull, SetNull);
    }
   }
   OS_SRWMutexScopeEnter_R(stripe->rw_mutex);
   sent = FS_U2SEnqueueRequest(path, endt_us);
  }
  if(good)
  {
   break;
  }
  if(OS_TimeMicroseconds() >= endt_us)
  {
   break;
  }
  OS_ConditionVariableWaitSRW_R(stripe->cv, stripe->rw_mutex, endt_us);
 }
 return val;
}

////////////////////////////////
//~ rjf: Streaming Thread

internal B32
FS_U2SEnqueueRequest(String8 path, U64 endt_us)
{
 B32 sent = 0;
 OS_MutexScope(fs_shared->u2s_ring_mutex) for(;;)
 {
  U64 unconsumed_size = fs_shared->u2s_ring_write_pos - fs_shared->u2s_ring_read_pos;
  U64 available_size = fs_shared->u2s_ring_size - unconsumed_size;
  if(available_size >= sizeof(U64) + path.size)
  {
   sent = 1;
   fs_shared->u2s_ring_write_pos += RingWriteStruct(fs_shared->u2s_ring_base, fs_shared->u2s_ring_size, fs_shared->u2s_ring_write_pos, &path.size);
   fs_shared->u2s_ring_write_pos += RingWrite(fs_shared->u2s_ring_base, fs_shared->u2s_ring_size, fs_shared->u2s_ring_write_pos, path.str, path.size);
   fs_shared->u2s_ring_write_pos += 7;
   fs_shared->u2s_ring_write_pos -= fs_shared->u2s_ring_write_pos%8;
   break;
  }
  if(OS_TimeMicroseconds() >= endt_us)
  {
   break;
  }
  OS_ConditionVariableWait(fs_shared->u2s_ring_cv, fs_shared->u2s_ring_mutex, endt_us);
 }
 if(sent)
 {
  OS_ConditionVariableSignalAll(fs_shared->u2s_ring_cv);
 }
 return sent;
}

internal String8
FS_U2SDequeueRequest(Arena *arena)
{
 String8 result = {0};
 OS_MutexScope(fs_shared->u2s_ring_mutex) for(;;)
 {
  U64 unconsumed_size = fs_shared->u2s_ring_write_pos - fs_shared->u2s_ring_read_pos;
  if(unconsumed_size >= sizeof(U64))
  {
   fs_shared->u2s_ring_read_pos += RingReadStruct(fs_shared->u2s_ring_base, fs_shared->u2s_ring_size, fs_shared->u2s_ring_read_pos, &result.size);
   result.str = PushArrayNoZero(arena, U8, result.size);
   fs_shared->u2s_ring_read_pos += RingRead(fs_shared->u2s_ring_base, fs_shared->u2s_ring_size, fs_shared->u2s_ring_read_pos, result.str, result.size);
   fs_shared->u2s_ring_read_pos += 7;
   fs_shared->u2s_ring_read_pos -= fs_shared->u2s_ring_read_pos%8;
   break;
  }
  OS_ConditionVariableWait(fs_shared->u2s_ring_cv, fs_shared->u2s_ring_mutex, U64Max);
 }
 OS_ConditionVariableSignalAll(fs_shared->u2s_ring_cv);
 return result;
}

internal void
FS_StreamThreadEntryPoint(void *p)
{
 SetThreadNameF("[FS] Streamer #%I64u", (U64)p);
 ProfThreadName("[FS] Streamer #%I64u", (U64)p);
 for(;;)
 {
  Temp scratch = ScratchBegin(0, 0);
  
  //- rjf: get next path
  String8 path = FS_U2SDequeueRequest(scratch.arena);
  U64 hash = FS_HashFromString(path);
  U64 slot_idx = hash%fs_shared->slots_count;
  U64 stripe_idx = slot_idx%fs_shared->stripes_count;
  FS_Slot *slot = &fs_shared->slots[slot_idx];
  FS_Stripe *stripe = &fs_shared->stripes[stripe_idx];
  
  //- rjf: find node, take task
  B32 good_task = 0;
  OS_SRWMutexScope_R(stripe->rw_mutex)
  {
   for(FS_Node *n = slot->first; n != 0; n = n->hash_next)
   {
    if(Str8Match(n->path, path, 0))
    {
     good_task = !AtomicEvalCompareExchange64(&n->task_taken, 1, 0);
     break;
    }
   }
  }
  
  //- rjf: load file
  Arena *data_arena = 0;
  String8 data = {0};
  U64 timestamp = 0;
  if(good_task)
  {
   OS_Handle file = OS_FileOpen(OS_AccessFlag_Read, path);
   OS_FileAttributes atts = OS_AttributesFromFile(file);
   U64 data_arena_size = atts.size;
   data_arena_size += Megabytes(1)-1;
   data_arena_size -= data_arena_size%Megabytes(1);
   data_arena = ArenaAlloc(data_arena_size);
   data = OS_FileRead(data_arena, file, R1U64(0, atts.size));
   timestamp = atts.last_modified;
   OS_FileClose(file);
  }
  
  //- rjf: submit data into hash store
  U128 data_hash = {0};
  if(good_task)
  {
   data_hash = C_SubmitData(&data_arena, data);
  }
  
  //- rjf: store into cache
  if(good_task) OS_SRWMutexScope_W(stripe->rw_mutex)
  {
   for(FS_Node *n = slot->first; n != 0; n = n->hash_next)
   {
    if(Str8Match(n->path, path, 0))
    {
     n->val.hash = data_hash;
     n->val.timestamp = timestamp;
     n->task_taken = 0;
     break;
    }
   }
  }
  
  //- rjf: broadcast
  if(good_task)
  {
   OS_ConditionVariableSignalAll(stripe->cv);
  }
  
  ScratchEnd(scratch);
 }
}

////////////////////////////////
//~ rjf: Scanner Thread

internal void
FS_ScannerThreadEntryPoint(void *p)
{
 SetThreadName(Str8Lit("[FS] Scanner"));
 ProfThreadName("[FS] Scanner");
 for(;;)
 {
  for(U64 slot_idx = 0; slot_idx < fs_shared->slots_count; slot_idx += 1)
  {
   FS_Slot *slot = &fs_shared->slots[slot_idx];
   U64 stripe_idx = slot_idx%fs_shared->stripes_count;
   FS_Stripe *stripe = &fs_shared->stripes[stripe_idx];
   OS_SRWMutexScope_R(stripe->rw_mutex)
   {
    for(FS_Node *n = slot->first; n != 0; n = n->hash_next)
    {
     OS_FileAttributes atts = OS_AttributesFromFilePath(n->path);
     if(atts.last_modified != n->val.timestamp)
     {
      FS_U2SEnqueueRequest(n->path, U64Max);
     }
    }
   }
  }
  OS_Sleep(100);
 }
}
