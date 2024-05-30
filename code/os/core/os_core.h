#ifndef OS_CORE_H
#define OS_CORE_H

////////////////////////////////
//~ rjf: Basics

typedef struct OS_InitReceipt OS_InitReceipt;
struct OS_InitReceipt
{
 U64 u64[1];
};

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
 U64 u64[1];
};

typedef U32 OS_AccessFlags;
enum
{
 OS_AccessFlag_Read      = (1<<0),
 OS_AccessFlag_Write     = (1<<1),
 OS_AccessFlag_Execute   = (1<<2),
 OS_AccessFlag_CreateNew = (1<<3),
 OS_AccessFlag_Shared    = (1<<4),
};

////////////////////////////////
//~ rjf: Errors

typedef enum OS_ErrorCode
{
 OS_ErrorCode_Null,
 OS_ErrorCode_COUNT
}
OS_ErrorCode;

typedef struct OS_Error OS_Error;
struct OS_Error
{
 OS_Error *next;
 OS_ErrorCode code;
};

typedef struct OS_ErrorList OS_ErrorList;
struct OS_ErrorList
{
 OS_Error *first;
 OS_Error *last;
 U64 count;
};

////////////////////////////////
//~ rjf: Timestamps

typedef U64 OS_Timestamp;

////////////////////////////////
//~ rjf: Filesystem Types

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
 U8 opaque[1024];
};

typedef U32 OS_FileFlags;
enum
{
 OS_FileFlag_Directory = (1<<0),
};

typedef struct OS_FileAttributes OS_FileAttributes;
struct OS_FileAttributes
{
 OS_FileFlags flags;
 U64 size;
 OS_Timestamp last_modified;
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{
 String8 name;
 OS_FileAttributes attributes;
};

typedef struct OS_FileInfoNode OS_FileInfoNode;
struct OS_FileInfoNode
{
 OS_FileInfoNode *next;
 OS_FileInfo v;
};

typedef struct OS_FileInfoList OS_FileInfoList;
struct OS_FileInfoList
{
 OS_FileInfoNode *first;
 OS_FileInfoNode *last;
 U64 count;
};

////////////////////////////////
//~ rjf: System Path Types

typedef enum OS_SystemPathKind
{
 OS_SystemPathKind_Null,
 OS_SystemPathKind_Initial,
 OS_SystemPathKind_Current,
 OS_SystemPathKind_Binary,
 OS_SystemPathKind_AppData,
 OS_SystemPathKind_COUNT,
}
OS_SystemPathKind;

////////////////////////////////
//~ rjf: Thread & Process Types

typedef void OS_ThreadFunction(void *params);

typedef struct OS_ProcessStatus OS_ProcessStatus;
struct OS_ProcessStatus
{
 B8 launch_failed;
 B8 running;
 B8 read_failed;
 B8 kill_failed;
 B8 was_killed;
 U32 exit_code;
};

typedef struct OS_Stripe OS_Stripe;
struct OS_Stripe
{
 OS_Handle cv;
 OS_Handle mutex;
 Arena *arena;
};

typedef struct OS_StripeTable OS_StripeTable;
struct OS_StripeTable
{
 U64 count;
 OS_Stripe *stripes;
};

////////////////////////////////
//~ rjf: Helpers

//- rjf: handle type functions
internal OS_Handle OS_HandleZero(void);
internal B32 OS_HandleMatch(OS_Handle a, OS_Handle b);

//- rjf: file info list functions
internal void OS_FileInfoListPush(Arena *arena, OS_FileInfoList *list, OS_FileInfo *v);

//- rjf: path normalizations
internal String8 OS_NormalizedPathFromStr8(Arena *arena, String8 string);

//- rjf: file system interaction bundlers
internal String8 OS_DataFromFilePath(Arena *arena, String8 path);
internal void OS_WriteDataToFilePath(String8 path, String8List data);
internal B32 OS_FileExistsAtPath(String8 path);
internal OS_FileInfoList OS_FileInfoListFromPath(Arena *arena, String8 path);

//- rjf: stripe table
internal OS_StripeTable *OS_StripeTableAlloc(Arena *arena, U64 count);
internal void OS_StripeTableRelease(OS_StripeTable *table);

//- rjf: timestamp <-> date time
internal DateTime OS_DateTimeFromTimestamp(OS_Timestamp timestamp);
internal OS_Timestamp OS_TimestampFromDateTime(DateTime date_time);

////////////////////////////////
//~ rjf: @os_per_backend Main Layer Initialization

internal OS_InitReceipt OS_Init(void);

////////////////////////////////
//~ rjf: @os_per_backend Aborting

internal void OS_Abort(void);

////////////////////////////////
//~ rjf: @os_per_backend System Info

internal String8 OS_StringFromSystemPathKind(Arena *arena, OS_SystemPathKind path);
internal U64 OS_PageSize(void);
internal F32 OS_CaretBlinkTime(void);
internal F32 OS_DoubleClickTime(void);
internal U64 OS_LogicalProcessorCount(void);

////////////////////////////////
//~ rjf: @os_per_backend Memory

internal void *OS_Reserve(U64 size);
internal void  OS_Release(void *ptr, U64 size);
internal void  OS_Commit(void *ptr, U64 size);
internal void  OS_Decommit(void *ptr, U64 size);
internal void  OS_Protect(void *ptr, U64 size, OS_AccessFlags flags);

////////////////////////////////
//~ rjf: @os_per_backend Libraries

internal OS_Handle     OS_LibraryOpen(String8 path);
internal void          OS_LibraryClose(OS_Handle handle);
internal VoidFunction *OS_LibraryLoadFunction(OS_Handle handle, String8 name);

////////////////////////////////
//~ rjf: @os_per_backend File System

//- rjf: handle-based operations
internal OS_Handle         OS_FileOpen(OS_AccessFlags access_flags, String8 path);
internal void              OS_FileClose(OS_Handle file);
internal String8           OS_FileRead(Arena *arena, OS_Handle file, Rng1U64 range);
internal void              OS_FileWrite(OS_Handle file, U64 off, String8List data);
internal B32               OS_FileIsValid(OS_Handle file);
internal OS_FileAttributes OS_AttributesFromFile(OS_Handle file);

//- rjf: path-based operations
internal void OS_DeleteFile(String8 path);
internal void OS_MoveFile(String8 dst_path, String8 src_path);
internal B32  OS_CopyFile(String8 dst_path, String8 src_path);
internal B32  OS_MakeDirectory(String8 path);

//- rjf: file system introspection
internal OS_FileIter *     OS_FileIterBegin(Arena *arena, String8 path);
internal B32               OS_FileIterNext(Arena *arena, OS_FileIter *it, OS_FileInfo *out_info);
internal void              OS_FileIterEnd(OS_FileIter *it);
internal OS_FileAttributes OS_AttributesFromFilePath(String8 path);

////////////////////////////////
//~ rjf: @os_per_backend Time

internal DateTime OS_DateTimeCurrent(void);
internal U64 OS_TimeMicroseconds(void);
internal void OS_Sleep(U64 milliseconds);
internal void OS_Wait(U64 end_time_microseconds);

////////////////////////////////
//~ rjf: @os_per_backend Threads & Synchronization Primitives

//- rjf: thread controls
internal U64 OS_TID(void);
internal void OS_SetThreadName(String8 name);
internal OS_Handle OS_ThreadStart(void *params, OS_ThreadFunction *func);
internal void OS_ThreadJoin(OS_Handle thread);
internal void OS_ThreadDetach(OS_Handle thread);

//- rjf: mutexes
internal OS_Handle OS_MutexAlloc(void);
internal void OS_MutexRelease(OS_Handle mutex);
internal void OS_MutexScopeEnter(OS_Handle mutex);
internal void OS_MutexScopeLeave(OS_Handle mutex);
#define OS_MutexScope(m) DeferLoop(OS_MutexScopeEnter(m), OS_MutexScopeLeave(m))

//- rjf: slim reader/writer mutexes
internal OS_Handle OS_SRWMutexAlloc(void);
internal void OS_SRWMutexRelease(OS_Handle mutex);
internal void OS_SRWMutexScopeEnter_W(OS_Handle mutex);
internal void OS_SRWMutexScopeLeave_W(OS_Handle mutex);
internal void OS_SRWMutexScopeEnter_R(OS_Handle mutex);
internal void OS_SRWMutexScopeLeave_R(OS_Handle mutex);
#define OS_SRWMutexScope_W(m) DeferLoop(OS_SRWMutexScopeEnter_W(m), OS_SRWMutexScopeLeave_W(m))
#define OS_SRWMutexScope_R(m) DeferLoop(OS_SRWMutexScopeEnter_R(m), OS_SRWMutexScopeLeave_R(m))

//- rjf: semaphores
internal OS_Handle OS_SemaphoreAlloc(U32 initial_count, U32 max_count);
internal void OS_SemaphoreRelease(OS_Handle handle);
internal B32 OS_SemaphoreWait(OS_Handle handle, U32 max_milliseconds);
internal U64 OS_SemaphoreSignal(OS_Handle handle);

//- rjf: condition variables
internal OS_Handle OS_ConditionVariableAlloc(void);
internal void OS_ConditionVariableRelease(OS_Handle cv);
internal B32 OS_ConditionVariableWait(OS_Handle cv, OS_Handle mutex, U64 endt_us);
internal B32 OS_ConditionVariableWaitSRW_W(OS_Handle cv, OS_Handle mutex, U64 endt_us);
internal B32 OS_ConditionVariableWaitSRW_R(OS_Handle cv, OS_Handle mutex, U64 endt_us);
internal void OS_ConditionVariableSignal(OS_Handle cv);
internal void OS_ConditionVariableSignalAll(OS_Handle cv);

////////////////////////////////
//~ rjf: @os_per_backend Child Processes

internal OS_Handle        OS_ProcessLaunch(String8 command, String8 working_directory);
internal void             OS_ProcessRelease(OS_Handle handle);
internal String8          OS_ProcessReadOutput(Arena *arena, OS_Handle process);
internal void             OS_ProcessKill(OS_Handle process);
internal U64              OS_PIDFromProcess(OS_Handle process);
internal OS_ProcessStatus OS_StatusFromProcess(OS_Handle process);

////////////////////////////////
//~ rjf: @os_per_backend Miscellaneous

internal void OS_GetEntropy(void *data, U64 size);

#endif // OS_CORE_H
