internal HtmlTokenNode*
html_token_push_list(Arena *arena, HtmlTokenList *list, HtmlToken token)
{
  HtmlTokenNode *node = push_array_no_zero(arena, HtmlTokenNode, 1);
  SLLPush(list->first, list->last, node);
  list->count += 1;  
  return node;
}

internal void 
html_parse(Arena *arena, OS_FileInfoList *list)
{
  for(OS_FileInfoNode *node = list->first;
      node != 0;
      node = node->next
  )
  {
    OS_Handle handle = os_file_open(arena, node->info.filename);
    if(os_handle_match(handle, os_handle_zero())) continue;
    Temp scratch = temp_begin(arena);
    U8 *memory = push_array_no_zero(scratch.arena, U8, 2000);
    U64 size = os_file_read(handle, rng_1u64(0, 2000), memory);
    temp_end(scratch);
  }
}