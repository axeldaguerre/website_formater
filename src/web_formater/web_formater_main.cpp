#include <stdio.h>
#if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "html/html_inc.h"
#include "third_party/stb/stb_sprintf.h"

#include "os/os_inc.c"
#include "base/base_inc.c"
#include "html/html_inc.c"

#if OS_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Arena *perm_arena = arena_allocate__sized(GB(2), MB(128));
  WCHAR *command_line = GetCommandLineW();
  int argc;
  WCHAR **argv_16 = CommandLineToArgvW(command_line, &argc);
  char **argv = push_array(perm_arena, char *, argc);  
  String8 root_path = os_current_directory(perm_arena);
  for(int i = 1;i < argc; ++i) 
  {
    String16 argv16 = str16_cstring((U16*)argv_16[i]);
    String8  argv8 = str8_from_16(perm_arena, argv16);
    argv[i] = (char*)argv8.str;
  }
  
  if(argc > 1) 
  {
    String8 target_path = str8((U8*)argv[1], cstr8_length((U8*)argv[1]));
    OS_FileInfoList *info_list = push_array(perm_arena, OS_FileInfoList, 1);
    // TODO: replace by the OS_FileInfoListFromPath() root basis  
    os_push_files_infos(perm_arena, target_path, OS_FileIterFlag_SkipHiddenFiles, info_list);
    
    String8 full_errors = html_parse(perm_arena, info_list);
    String8 log_file_path = push_str8_cat(perm_arena, root_path, str8_lit("\\logs.txt"));
    if(argc >= 2)
    {
        log_file_path = push_str8_cat(perm_arena, str8((U8*)argv[2], cstr8_length((U8*)argv[2])), str8_lit("\\logs.txt"));
    }
    os_write_data_to_file_path(perm_arena, log_file_path, full_errors);
  }
  
  return 0;
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{  
  return 0;
}

#endif
