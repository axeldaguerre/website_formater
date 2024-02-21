#include <Windows.h>

typedef struct W32_FileIter W32_FileIter; 
struct W32_FileIter
{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
};


internal void *        os_reserve(U64 size);
internal B32           os_commit(void *ptr, U64 size);
internal String8       os_string_from_system_path(Arena *arena);
internal void          os_release(void *ptr, U64 size);
internal OS_Handle     os_file_open(Arena *arena, String8 path);
internal void          os_file_write(OS_Handle file, Rng1U64 rng, void *data);
internal U64           os_file_read(OS_Handle file, Rng1U64 rng, void *out_data);
internal OS_FileIter * os_file_iter_begin(Arena *arena, String8 query, OS_FileIterFlags flags);
internal B32           os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *out_info);
internal void          os_file_iter_end(OS_FileIter *iter);