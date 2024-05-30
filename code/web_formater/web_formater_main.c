#include <stdio.h>

#if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__) || defined(__linux__)
#  define OS_LINUX 1
#endif

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "html/html_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "html/html_inc.c"

#include "os/core/os_entry_point.c"

/*
  TODOS:
     - @Computation: Tokenizer on comment don't trigger which 
     - @Error on  no space attributes, this parser doesn't care but others do.
     - @Error on comment is not ok, it seems to not catch a bad constructed comment 
     
*/
internal void
EntryPoint(CmdLine *cmdln)
{
  Arena *perm_arena = ArenaAllocDefault();
  WCHAR *command_line = GetCommandLineW();
  String8 website_path = Str8Lit("C:/Users/axeld/Documents/Projects/Projects_Repositories/axeldaguerre/public/");
  
  String8List *path_list = PushArray(perm_arena, String8List, 1);
  typedef struct Task Task;
  struct Task
  {
    Task *next;
    String8 dir;
  };
  Task start_task = {0, website_path};
  Task *first_task = &start_task;
  Task *last_task = &start_task;
  for(Task *task = first_task; task != 0; task = task->next)
  {
    OS_FileIter *it = OS_FileIterBegin(perm_arena, task->dir);
    for(OS_FileInfo info = {0}; OS_FileIterNext(perm_arena, it,  &info);)
    {
      if(info.attributes.flags == OS_FileFlag_Directory)
      {
        String8 path = PushStr8F(perm_arena, "%S%S/", task->dir, info.name);
        Task *task = PushArray(perm_arena, Task, 1);
        QueuePush(first_task, last_task, task);
        task->dir = path;
      }
      else
      {
        String8 path = PushStr8F(perm_arena, "%S%S", task->dir, info.name);
        Str8ListPush(perm_arena, path_list, path);
      }
    }
  }
  String8List *err_msg_list = PushArray(perm_arena, String8List, 1);
  for(String8Node *n = path_list->first; n != 0; n = n->next)
  {
    if(!Str8Match(Str8SkipLastDot(n->string), Str8Lit("html"), MatchFlag_CaseInsensitive))
    {
      continue;
    }
    
    String8 data = OS_DataFromFilePath(perm_arena, n->string);
    HTMLParser *parser = HTML_InitParser(perm_arena, data, 0);
    HTMLElementNode doc_el = HTML_ParseDocument(perm_arena, parser);
    String8 errors = HTML_GetErrorMsg(perm_arena, parser);
    if(errors.size)
    {
      String8 prefix = PushStr8F(perm_arena, "%S%S", Str8Lit("File path:"), n->string);
      Str8ListPush(perm_arena, err_msg_list, prefix);
      Str8ListPush(perm_arena, err_msg_list, errors);
    }    
  }
  OS_WriteDataToFilePath(Str8Lit("C:\\Users\\axeld\\Documents\\Projects\\Projects_Repositories\\axeldaguerre\\log_errors.txt"), *err_msg_list);
  
}