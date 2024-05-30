////////////////////////////////
//~ rjf: Thread Context Construction & Equipping

internal ThreadCtx
ThreadCtxAlloc(void)
{
 ThreadCtx tctx = {0};
 for(U64 arena_idx = 0; arena_idx < ArrayCount(tctx.scratch_arenas); arena_idx += 1)
 {
  tctx.scratch_arenas[arena_idx] = ArenaAlloc(Gigabytes(8));
 }
 tctx.log_arena = ArenaAlloc(Megabytes(64));
 return tctx;
}

internal void
ThreadCtxRelease(ThreadCtx *tctx)
{
 for(U64 arena_idx = 0; arena_idx < ArrayCount(tctx->scratch_arenas); arena_idx += 1)
 {
  ArenaRelease(tctx->scratch_arenas[arena_idx]);
 }
}

#if BUILD_CORE
per_thread ThreadCtx *tl_tctx = 0;

no_name_mangle void
SetThreadCtx(ThreadCtx *tctx)
{
 tl_tctx = tctx;
}

no_name_mangle ThreadCtx *
GetThreadCtx(void)
{
 return tl_tctx;
}
#endif

////////////////////////////////
//~ rjf: Thread Metadata

internal void
SetThreadName(String8 string)
{
 ThreadCtx *tctx = GetThreadCtx();
 tctx->thread_name_size = Min(string.size, sizeof(tctx->thread_name));
 MemoryCopy(tctx->thread_name, string.str, tctx->thread_name_size);
#if defined(ThreadCtxImpl_SetThreadName)
 ThreadCtxImpl_SetThreadName(string);
#endif
}

internal void
SetThreadNameF(char *fmt, ...)
{
 Temp scratch = ScratchBegin(0, 0);
 va_list args;
 va_start(args, fmt);
 String8 string = PushStr8FV(scratch.arena, fmt, args);
 SetThreadName(string);
 va_end(args);
 ScratchEnd(scratch);
}

internal String8
GetThreadName(void)
{
 ThreadCtx *tctx = GetThreadCtx();
 String8 result = Str8(tctx->thread_name, tctx->thread_name_size);
 return result;
}

internal B32
IsMainThread()
{
 ThreadCtx *tctx = GetThreadCtx();
 return tctx->is_main_thread;
}

//- rjf: source code location set helper
internal void
SetThreadFileAndLine_(char *file, int line)
{
 ThreadCtx *tctx = GetThreadCtx();
 tctx->file_name = file;
 tctx->line_number = line;
}

////////////////////////////////
//~ rjf: Scratch Memory

internal Temp
ScratchBegin(Arena **conflicts, U64 conflict_count)
{
 Temp scratch = {0};
 ThreadCtx *tctx = GetThreadCtx();
 for(U64 tctx_idx = 0; tctx_idx < ArrayCount(tctx->scratch_arenas); tctx_idx += 1)
 {
  B32 is_conflicting = 0;
  for(Arena **conflict = conflicts; conflict < conflicts+conflict_count; conflict += 1)
  {
   if(*conflict == tctx->scratch_arenas[tctx_idx])
   {
    is_conflicting = 1;
    break;
   }
  }
  if(is_conflicting == 0)
  {
   scratch.arena = tctx->scratch_arenas[tctx_idx];
   scratch.pos = scratch.arena->pos;
   break;
  }
 }
 return scratch;
}

////////////////////////////////
//~ rjf: Log Building

internal void
PushLogScope(String8 name)
{
 ThreadCtx *tctx = GetThreadCtx();
 LogNode *node = push_array(tctx->log_arena, LogNode, 1);
 if(node != 0)
 {
  node->string = PushStr8Copy(tctx->log_arena, name);
  node->parent = tctx->log_active_parent;
  if(!node->parent)
  {
   tctx->log_root = node;
  }
  else
  {
   QueuePush(node->parent->first, node->parent->last, node);
  }
  tctx->log_active_parent = node;
 }
}

internal void
PushLogScopeF(char *fmt, ...)
{
 Temp scratch = ScratchBegin(0, 0);
 va_list args;
 va_start(args, fmt);
 String8 string = PushStr8FV(scratch.arena, fmt, args);
 PushLogScope(string);
 va_end(args);
 ScratchEnd(scratch);
}

function	void 
PopLogScope(void)
{
 ThreadCtx *tctx = GetThreadCtx();
 if(tctx->log_active_parent)
 {
  tctx->log_active_parent = tctx->log_active_parent->parent;
 }
}

internal void
PushLogMsg(LogMsgKind kind, String8 string)
{
 
}

internal void
PushLogMsgF(LogMsgKind kind, char *fmt, ...)
{
 Temp scratch = ScratchBegin(0, 0);
 va_list args;
 va_start(args, fmt);
 String8 string = PushStr8FV(scratch.arena, fmt, args);
 PushLogMsg(kind, string);
 va_end(args);
 ScratchEnd(scratch);
}

////////////////////////////////
//~ rjf: Log Consumption

internal LogMsgArray
ConsumeLog(Arena *arena, LogMsgKind min_kind)
{
 LogMsgArray msgs = {0};
 return msgs;
}
