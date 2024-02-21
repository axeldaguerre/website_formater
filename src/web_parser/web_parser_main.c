#include <stdio.h>
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "html/html_base.h"

#include "os/os_inc.c"
#include "base/base_inc.c"
#include "html/html_base.c"

#if _WIN32
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Arena *perm_arena = arena_allocate__sized(GB(2), MB(128));  
  WCHAR *command_line = GetCommandLineW();
  int argc;
  WCHAR **argv_16 = CommandLineToArgvW(command_line, &argc);
  char **argv = push_array(perm_arena, char *, argc);  

  for(int i = 1;i < argc; ++i) {
    String16 argv16 = str16_cstring((U16*)argv_16[i]);
    String8  argv8 = str8_from_16(perm_arena, argv16);
    argv[i] = (char*)argv8.str;
  }
  
  if(argc > 1) {
    String8 target_path = str8((U8*)argv[1], cstr8_length((U8*)argv[1]));
    String8 target_path_wildcard = push_str8_cat(perm_arena, target_path, str8_lit("\\*"));
    
    OS_FileInfoList *info_list = push_array(perm_arena, OS_FileInfoList, 1);
    OS_FileIterFlags flags = OS_FileIterFlag_SkipHiddenFiles;
    os_push_files_infos(perm_arena, target_path_wildcard, flags, info_list);
    
    html_parse(perm_arena, info_list);
    int a = 0;
  }
  
  return 0;
}

#elif OS_LINUX

int main(int argument_count, char **arguments)
{  
  return 0;
}

#endif
