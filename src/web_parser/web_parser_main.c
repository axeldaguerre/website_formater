#include <stdio.h>
#if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
// TODO: use inc.h instead
#include "html/html_base.h"
#include "html/html_text_table.h"
// #include "html/html_inc.h"

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
    // use OS_FileIterFlag flags
    os_push_files_infos(perm_arena, target_path, OS_FileIterFlag_SkipHiddenFiles, info_list);
    String8 fullErrors = html_parse(perm_arena, info_list);
    String8 path_logs = push_str8_cat(perm_arena, target_path, str8_lit("/logs.txt")); 
    OS_Handle file = os_file_open(perm_arena, path_logs, OS_AccessFlag_Write);
    os_file_write(file, r1u64(0, fullErrors.size), fullErrors.str);
  }
  
  return 0;
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{  
  return 0;
}

#endif
