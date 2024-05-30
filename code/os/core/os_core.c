////////////////////////////////
//~ rjf: Helpers

//- rjf: handle type functions

internal OS_Handle
OS_HandleZero(void)
{
 OS_Handle handle = {0};
 return handle;
}

internal B32
OS_HandleMatch(OS_Handle a, OS_Handle b)
{
 return a.u64[0] == b.u64[0];
}

//- rjf: file info list functions

internal void
OS_FileInfoListPush(Arena *arena, OS_FileInfoList *list, OS_FileInfo *v)
{
 OS_FileInfoNode *n = PushArray(arena, OS_FileInfoNode, 1);
 MemoryCopyStruct(&n->v, v);
 QueuePush(list->first, list->last, n);
 list->count += 1;
}

//- rjf: path normalizations

internal String8
OS_NormalizedPathFromStr8(Arena *arena, String8 string)
{
 Temp scratch = ScratchBegin(&arena, 1);
 String8 source = OS_StringFromSystemPathKind(scratch.arena, OS_SystemPathKind_Current);
 String8 result = NormalizedPathFromStr8(arena, source, string);
 ScratchEnd(scratch);
 return result;
}

//- rjf: file system interaction bundlers

internal String8
OS_DataFromFilePath(Arena *arena, String8 path)
{
 String8 result = {0};
 OS_Handle file = OS_FileOpen(OS_AccessFlag_Read|OS_AccessFlag_Shared, path);
 OS_FileAttributes atts = OS_AttributesFromFile(file);
 Rng1U64 range = R1U64(0, atts.size);
 result = OS_FileRead(arena, file, range);
 OS_FileClose(file);
 return result;
}

internal void
OS_WriteDataToFilePath(String8 path, String8List data)
{
 OS_Handle file = OS_FileOpen(OS_AccessFlag_Write, path);
 OS_FileWrite(file, 0, data);
 OS_FileClose(file);
}

internal B32
OS_FileExistsAtPath(String8 path)
{
 OS_Handle file = OS_FileOpen(OS_AccessFlag_Read|OS_AccessFlag_Shared, path);
 B32 result = OS_FileIsValid(file);
 OS_FileClose(file);
 return result;
}

internal OS_FileInfoList
OS_FileInfoListFromPath(Arena *arena, String8 path)
{
 OS_FileInfoList list = {0};
 Temp scratch = ScratchBegin(&arena, 1);
 OS_FileIter *it = OS_FileIterBegin(scratch.arena, path);
 for(OS_FileInfo info = {0}; OS_FileIterNext(arena, it, &info);)
 {
  OS_FileInfoListPush(arena, &list, &info);
 }
 OS_FileIterEnd(it);
 ScratchEnd(scratch);
 return list;
}

//- rjf: stripe table

internal OS_StripeTable *
OS_StripeTableAlloc(Arena *arena, U64 count)
{
 OS_StripeTable *table = PushArray(arena, OS_StripeTable, 1);
 table->count = count;
 table->stripes = PushArray(arena, OS_Stripe, count);
 for(U64 idx = 0; idx < count; idx += 1)
 {
  table->stripes[idx].cv    = OS_ConditionVariableAlloc();
  table->stripes[idx].mutex = OS_MutexAlloc();
  table->stripes[idx].arena = ArenaAlloc(Gigabytes(1));
 }
 return table;
}

internal void
OS_StripeTableRelease(OS_StripeTable *table)
{
 for(U64 idx = 0; idx < table->count; idx += 1)
 {
  OS_ConditionVariableRelease(table->stripes[idx].cv);
  OS_MutexRelease(table->stripes[idx].mutex);
  ArenaRelease(table->stripes[idx].arena);
 }
}

//- rjf: timestamp <-> date time

internal DateTime
OS_DateTimeFromTimestamp(OS_Timestamp timestamp)
{
 DateTime dt = {0};
 {
  // NOTE(rjf): origin => 1/1/1601
  U64 ns_since_origin = timestamp;
  U64 us_since_origin = ns_since_origin/1000;
  U64 ms_since_origin = us_since_origin/1000;
  U64 s_since_origin  = ms_since_origin/1000;
  U64 m_since_origin  = s_since_origin/60;
  U64 h_since_origin  = m_since_origin/60;
  U64 d_since_origin  = h_since_origin/24;
 }
 return dt;
}

internal OS_Timestamp
OS_TimestampFromDateTime(DateTime date_time)
{
 OS_Timestamp timestamp = 0;
 {
  
 }
 return timestamp;
}
