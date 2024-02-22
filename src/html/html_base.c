internal HTMLTokenNode*
html_token_push_list(Arena *arena, HTMLTokenList *list, HTMLToken token)
{
  HTMLTokenNode *node = push_array_no_zero(arena, HTMLTokenNode, 1);
  SLLPush(list->first, list->last, node);
  node->token = token;
  ++list->count;
  return node;
}

internal B32
html_is_in_bounds(HTMLParser *parser)
{
  return (parser->at <= parser->total_bytes);
}

internal B32
html_is_parsing(HTMLParser *parser)
{
  B32 result = !parser->on_error && html_is_in_bounds(parser);
  return result;
}

internal HTMLToken
html_get_token_next(Arena *arena, HTMLParser *parser, String8 *contents)
{
  HTMLToken result = {0};
  U64 at = parser->at;
  
  while(contents->str[at] == ' '){
    ++at;
  }
  
  if(html_is_in_bounds(parser)){
    U8 value = contents->str[at];
    ++at;
    switch(value)
    {
      case '<': { result.tag = HTMLTagType_h1;} break;
    }
  }
        
  parser->at = at;
  return result;
}

internal void 
html_parse(Arena *arena, OS_FileInfoList *info_list)
{
  for(OS_FileInfoNode *node = info_list->first;
      node != 0;
      node = node->next) 
  {    
    B32 is_html = str8_match(node->info.props.extension, str8_lit("html"), StringMatchFlag_CaseInsensitive);
    
    if(is_html)
    {
      Temp scratch = temp_begin(arena);

      U64 size_file = node->info.props.size;
      OS_Handle handle = os_file_open(arena, node->info.filename);
      if(os_handle_match(handle, os_handle_zero())) continue;

      U8 *memory = push_array_no_zero(scratch.arena, U8, size_file);
      U64 size = os_file_read(handle, rng_1u64(0, size_file), memory);
      String8 contents = str8(memory, size);

      HTMLParser parser = {0};
      parser.total_bytes = size;

      HTMLTokenList *token_list = push_array(scratch.arena, HTMLTokenList, 1);
      while(html_is_parsing(&parser))
      {
        HTMLToken token = html_get_token_next(scratch.arena, &parser, &contents);
        HTMLTokenNode *token_node = html_token_push_list(scratch.arena, token_list, token);
      }
      temp_end(scratch);
    }
  }
}