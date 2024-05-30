#ifndef OS_CORE_WIN32_H
#define OS_CORE_WIN32_H

#pragma push_macro("function")
#undef function
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#pragma pop_macro("function")

////////////////////////////////
//~ rjf: Processes / Threads

typedef struct OS_W32_Process OS_W32_Process;
struct OS_W32_Process
{
 OS_W32_Process *next;
 HANDLE parent_read;
 PROCESS_INFORMATION info;
 OS_ProcessStatus status;
};

typedef struct OS_W32_Thread OS_W32_Thread;
struct OS_W32_Thread
{
 OS_W32_Thread *next;
 HANDLE handle;
 DWORD thread_id;
 void *params;
 OS_ThreadFunction *func;
};

////////////////////////////////
//~ rjf: Synchronization Primitives

typedef struct OS_W32_CriticalSection OS_W32_CriticalSection;
struct OS_W32_CriticalSection
{
 OS_W32_CriticalSection *next;
 CRITICAL_SECTION base;
};

typedef struct OS_W32_SRWLock OS_W32_SRWLock;
struct OS_W32_SRWLock
{
 OS_W32_SRWLock *next;
 SRWLOCK lock;
};

typedef struct OS_W32_ConditionVariable OS_W32_ConditionVariable;
struct OS_W32_ConditionVariable
{
 OS_W32_ConditionVariable *next;
 CONDITION_VARIABLE base;
};

////////////////////////////////
//~ rjf: File Find Data

typedef struct OS_W32_FileFindData OS_W32_FileFindData;
struct OS_W32_FileFindData
{
 HANDLE handle;
 B32 returned_first;
 WIN32_FIND_DATAW find_data;
};
StaticAssert(sizeof(OS_W32_FileFindData) <= sizeof(OS_FileIter), os_w32_file_find_data_size);

////////////////////////////////
//~ rjf: root_global State

typedef struct OS_W32_State OS_W32_State;
struct OS_W32_State
{
 Arena *arena;
 
 // rjf: top-level info
 HMODULE advapi_dll;
 SYSTEM_INFO system_info;
 LARGE_INTEGER counts_per_second;
 B32 granular_sleep_enabled;
 String8 initial_path;
 String8 binary_path;
 String8 app_data_path;
 
 // rjf: process entity state
 SRWLOCK process_srw_lock;
 Arena *process_arena;
 OS_W32_Process *first_free_process;
 
 // rjf: thread entity state
 SRWLOCK thread_srw_lock;
 Arena *thread_arena;
 OS_W32_Thread *first_free_thread;
 
 // rjf: critical section entity state
 SRWLOCK critical_section_srw_lock;
 Arena *critical_section_arena;
 OS_W32_CriticalSection *first_free_critical_section;
 
 // rjf: srw lock entity state
 SRWLOCK srw_lock_srw_lock;
 Arena *srw_lock_arena;
 OS_W32_SRWLock *first_free_srw_lock;
 
 // rjf: condition variable entity state
 SRWLOCK condition_variable_srw_lock;
 Arena *condition_variable_arena;
 OS_W32_ConditionVariable *first_free_condition_variable;
};

////////////////////////////////
//~ rjf: root_globals

root_global HINSTANCE os_w32_hinstance;
root_global OS_W32_State *os_w32_state;
root_global BOOL (*RtlGenRandom)(VOID *RandomBuffer, ULONG RandomBufferLength);

////////////////////////////////
//~ rjf: File System Helpers

internal OS_FileAttributes OS_W32_FileAttributesFromFindData(WIN32_FIND_DATAW find_data);

////////////////////////////////
//~ rjf: Entities

//- rjf: processes
internal OS_W32_Process *OS_W32_ProcessAlloc(void);
internal void OS_W32_ProcessRelease(OS_W32_Process *process);

//- rjf: threads
internal OS_W32_Thread *OS_W32_ThreadAlloc(void);
internal void OS_W32_ThreadRelease(OS_W32_Thread *thread);

//- rjf: critical sections
internal OS_W32_CriticalSection *OS_W32_CriticalSectionAlloc(void);
internal void OS_W32_CriticalSectionRelease(OS_W32_CriticalSection *critical_section);

//- rjf: srw locks
internal OS_W32_SRWLock *OS_W32_SRWLockAlloc(void);
internal void OS_W32_SRWLockRelease(OS_W32_SRWLock *srw_lock);

//- rjf: condition variables
internal OS_W32_ConditionVariable *OS_W32_ConditionVariableAlloc(void);
internal void OS_W32_ConditionVariableRelease(OS_W32_ConditionVariable *cv);

////////////////////////////////
//~ rjf: Thread Entry Point

internal DWORD OS_W32_ThreadEntryPoint(void *params);

#endif // OS_CORE_WIN32_H
