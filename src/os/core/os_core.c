internal OS_Handle
os_handle_zero()
{
  OS_Handle handle = {0};
  return handle;
}

internal B32
os_handle_match(OS_Handle a, OS_Handle b)
{
  return a.u64[0] == b.u64[0];
}

internal OS_FileInfoNode*
os_file_info_push_list(Arena *arena, OS_FileInfoList *list, OS_FileInfo *info)
{  
  OS_FileInfoNode *node = push_array(arena, OS_FileInfoNode, 1);
  node->info = *info;
  SLLPush(list->first, list->last, node);
  list->count += 1;
  return node;
}


internal OS_FileInfoArray
os_file_info_array_from_list(Arena *arena, OS_FileInfoList *list)
{
  OS_FileInfoArray array;
  array.count = list->count;  
  array.infos = push_array_no_zero(arena, OS_FileInfo, array.count);
  U64 i = 0;
  for(OS_FileInfoNode *n = list->first; n != 0; n = n->next, ++i) {
    array.infos[i] = n->info;
  }  
  return array;
}

internal OS_HandleNode*
os_handle_push_list(Arena *arena, OS_HandleList *list, OS_Handle *handle)
{
  OS_HandleNode *node = push_array_no_zero(arena, OS_HandleNode, list->count);
  SLLPush(list->first, list->last, node);
  list->count += 1;
  return node;
}

internal OS_HandleArray
os_handle_array_from_list(Arena *arena, OS_HandleList *list)
{
  OS_HandleArray array;
  array.count = list->count;
  array.handles = push_array_no_zero(arena, OS_Handle, array.count);  
  U64 i = 0;
  for(OS_HandleNode *n = list->first; n != 0; n = n->next, ++i) {
    array.handles[i] = n->handle;
  }
  return array;
}

internal OS_FileInfoList *
os_file_infos_join_list(Arena *arena, OS_FileInfoList *a, OS_FileInfoList *b)
{
  OS_FileInfoList *list = push_array(arena, OS_FileInfoList, 1);
  for(OS_FileInfoNode *node = a->first; node != 0; node = node->next)
  {
    SLLPush(list->first, list->last, node);
  }
  for(OS_FileInfoNode *node = b->first; node != 0; node = node->next)
  {
    SLLPush(list->first, list->last, node);
  }
  return list;
}

internal void
os_push_files_infos_from_folder(Arena *arena, String8 path, OS_FileIterFlags flags, OS_FileInfoList *list)
{ 
  String8 path_wildcard = push_str8_cat(arena, path, str8_lit("/*"));
  String16 path_16 = str16_from_str8(arena, path_wildcard);
  OS_FileIter *file_iter = os_file_iter_begin(arena, path_16, OS_FileIterFlag_None);
  if(file_iter) 
  {   
    do
    {      
      OS_FileInfo *info = push_array(arena, OS_FileInfo, 1);
      os_file_iter_next(arena, file_iter, info);
      if(info->props.flags & FilePropertyFlag_IsFolder)
      {
        String8 path_folder = push_str8_cat(arena, path, str8_lit("/"));
        String8 path_folder_wildcard = push_str8_cat(arena, path_folder, info->name);
        os_push_files_infos_from_folder(arena, path_folder_wildcard, flags, list);
      }
      else
      {
        os_file_info_push_list(arena, list, info);
      }
        
    } while(!(file_iter->flags & OS_FileIterFlag_Done));
    os_file_iter_end(file_iter);
    
  }
}

