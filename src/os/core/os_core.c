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
os_push_files_infos(Arena *arena, String8 query, OS_FileIterFlags flags, OS_FileInfoList *list)
{
  OS_FileIter *iter = os_file_iter_begin(arena, query, flags);
  
  String8 wildcard_split = str8_lit("*");
  String8List query_split_wildcard = str8_split(arena, query, wildcard_split, StringSplitFlag_None);
  
  if(iter) {
    do {
      OS_FileInfo *info = push_array(arena, OS_FileInfo, 1);
      
      if(os_file_iter_next(arena, iter, info)){
        if(info->props.flags & FilePropertyFlag_IsFolder){
          if(query_split_wildcard.count == 1){
            String8 folder_path = push_str8_cat(arena, query_split_wildcard.first->string, info->name);
            
            os_file_info_push_list(arena, list, info);
            Temp scratch = temp_begin(arena);
            os_push_files_infos(arena, push_str8_cat(scratch.arena, folder_path, str8_lit("\\*")), flags, list);
          }          
        } else {
          os_file_info_push_list(arena, list, info);
        }
      }
    } while(!(iter->flags & OS_FileIterFlag_Done));
    
    os_file_iter_end(iter);
  }
}
