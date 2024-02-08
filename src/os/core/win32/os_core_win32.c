#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#include <Windows.h>
// ================ Memory ==================
internal void * 
os_reserve(U64 size)
{ 
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

internal B32
os_commit(void *ptr, U64 size)
{   
  B32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal String8
os_string_from_system_path(Arena *arena)
{
    Temp scratch = temp_begin(arena);
    DWORD size = KB(32);
    U16 *buffer = push_array_no_zero(scratch.arena, U16, size);
    DWORD buffer_size = GetModuleFileNameW(0, (WCHAR*)buffer, size);
    String8 result = str8_from_16(scratch.arena, str16(buffer, buffer_size));
    temp_end(scratch);
    return result;
}

internal void 
os_release(void *ptr, U64 size)
{
    // Size not used in Windows but on used on others
    VirtualFree(ptr, 0, MEM_RELEASE);
}

// ================ File ==================
internal OS_Handle
os_file_open(String8 path)
{
    OS_Handle result = {0};
    HANDLE file = CreateFileW((WCHAR *)path.str, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(file != INVALID_HANDLE_VALUE)
    {
        result.u64[0] = (U64)file;
    }
    return result;
}

internal void
os_file_write(OS_Handle file, Rng1U64 rng, void *data)
{   
    if(os_handle_match(file, os_handle_zero())) return;

    HANDLE win32_handle = (HANDLE)file.u64[0];
    U64 to_write_size = (rng.max - rng.min);
    U64 total_write_size = 0;
    for(;total_write_size < to_write_size;)
    {
        void *memory = (void*)((U8*)data + total_write_size);
        U64 write_size = 0;
        U64 amount_64 = (to_write_size - total_write_size);
        // TODO(Axel): Test the maximum write size
        U32 amount_32 = u32_from_u64(amount_64);
        BOOL success  = WriteFile(win32_handle, memory, (DWORD)amount_32,  (DWORD*)&write_size, 0);
        if(success == 0) break;
        total_write_size += write_size;
    }
}

internal U64
os_file_read(OS_Handle file, Rng1U64 rng, void *out_data)
{
    if(os_handle_match(file, os_handle_zero())) { return 0; };
    HANDLE win32_handle = (HANDLE)file.u64[0];
    U64 file_size;
    GetFileSizeEx(win32_handle, (LARGE_INTEGER *)&file_size);
    Rng1U64 rng_clamped = rng_1u64( ClampTop(rng.min, file_size), ClampTop(rng.max, file_size) ); 
    U64 to_read = (rng_clamped.max - rng_clamped.min);
    U64 total_read_size = 0;
    for(;total_read_size < to_read;)
    {
        U64 amount_64 = (to_read - total_read_size);
        // TODO(Axel): Test the maximum read size
        U32 amount_32 = u32_from_u64(amount_64);
        // TODO(Axel): implement asyncronous (overlapped)
        DWORD bytes_read = 0;
        BOOL SUCCESS = ReadFile(win32_handle, (U8*)(out_data) + total_read_size, (DWORD)amount_32, &bytes_read, 0);
        if(SUCCESS ==0) break;
        total_read_size += bytes_read;
    }

    return total_read_size;
}

internal OS_FileIter *
os_file_iter_begin(Arena *arena, String16 path, OS_FileIterFlags flags)
{    
    OS_FileIter *iter = push_array(arena, OS_FileIter, 1);
    iter->flags = flags;
    W32_FileIter *win32_iter = (W32_FileIter*)iter->memory;
    win32_iter->hnd = FindFirstFileW((WCHAR*)path.str, &win32_iter->find_data);
    return iter;
}

internal B32
os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *out_info)
{
    B32 result = false;
    *out_info = {0};
    W32_FileIter *win32_file_iter = (W32_FileIter*)iter->memory;
    if(!(iter->flags & OS_FileIterFlag_Done)) 
    {      
      do {
          B32 is_valid = true;
          FilePropertiesFlags flags_properties = FilePropertyFlag_Unknown;
          DWORD attributes =  win32_file_iter->find_data.dwFileAttributes;
          WCHAR *name = win32_file_iter->find_data.cFileName;
          if(!(iter->flags & OS_FileIterFlag_Done) && name[0] == '.') 
          {
              is_valid = false;
          }
          if(attributes & FILE_ATTRIBUTE_DIRECTORY)
          {
            flags_properties |= FilePropertyFlag_IsFolder;
            if(iter->flags & OS_FileIterFlag_SkipFolders)
            {
                is_valid = false;
            }
          }
          else 
          {
              if(iter->flags & OS_FileIterFlag_SkipFiles) 
              {
                is_valid = false;
              }        
          }
          if(is_valid) 
          {
            result = true;
            out_info->name = str8_from_16(arena, str16((U16*)name, cstr16_length((U16*)name)));
            out_info->props.size = u64_from_high_low_u32(win32_file_iter->find_data.nFileSizeLow, win32_file_iter->find_data.nFileSizeHigh);
            out_info->props.flags = flags_properties;            
            if(!FindNextFileW(win32_file_iter->hnd, &win32_file_iter->find_data)) 
            {
                iter->flags |= OS_FileIterFlag_Done;
            }            
            break;
          }
      } while(FindNextFileW(win32_file_iter->hnd, &win32_file_iter->find_data));
    }
    if(!result)
    {
      iter->flags |= OS_FileIterFlag_Done;
    }
    return result;
}

internal void
os_file_iter_end(OS_FileIter *iter)
{
  W32_FileIter *w32_iter = (W32_FileIter*)iter->memory;
  FindClose(w32_iter->hnd);
}

