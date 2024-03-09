internal HTMLElementNode*
html_token_push(Arena *arena, HTMLElementList *list, HTMLElement element)
{
  HTMLElementNode *node = push_array_no_zero(arena, HTMLElementNode, 1);
  SLLPush(list->first, list->last, node);
  node->element = element;
  ++list->count;
  return node;
}

internal B32
html_is_in_bounds(HTMLParser *parser, U64 at)
{
  return (at < parser->string.size);
}

internal B32
html_is_parsing(HTMLParser *parser)
{
  B32 result = parser->error.type == HTMLErrorType_Null && html_is_in_bounds(parser, parser->at);
  return result;
}

// TODO: a flags system in order to do the flags = all_not_x
internal HTMLToken
html_get_token(HTMLParser *parser)
{
  HTMLToken result = {0};
  result.type = HTMLTokenType_dummy;
  U64 at = parser->at;
  result.string.str = parser->string.str + at;
  result.string.size = 1;
  switch(parser->string.str[at])
  {
    case '<': 
    {
      if(parser->string.str[at+1] == '/')
      {
        result.string.size = 2;
        result.type = HTMLTokenType_angle_bracket_open_then_slash;
      }
      else 
      {
        result.type = HTMLTokenType_angle_bracket_open;
      }
    } break;
    case '/': 
    {
      if(parser->string.str[at+1] == '>')
      {
        result.string.size = 2;
        result.type = HTMLTokenType_angle_slash_then_bracket_close;
      }
      else 
      {
        result.type = HTMLTokenType_slash;
      }
      
    } break;
    case '>': 
    { 
      result.type = HTMLTokenType_angle_bracket_close;
    } break;      
    case ' ': 
    { 
      result.type = HTMLTokenType_whitespace;
    } break;
    default :
    {
    } break;
  }
  at += result.string.size;
  parser->at = at;
  return result;
}

internal HTMLTag
html_get_next_tag(Arena *arena, HTMLParser *parser)
{
  HTMLToken token = {0};
  HTMLTag result = {0};
  Rng1U64 range = {0};
  HTMLTagEncoding encoding = {0};
  do 
  {
     token = html_get_token(parser);
  } while(token.type != HTMLTokenType_angle_bracket_open && token.type != HTMLTokenType_angle_bracket_open_then_slash);
  
  range.min = parser->at - token.string.size - 1;
  U64 count = 0;
  while(token.string.str[count] != '>') 
  {
    ++count;
  }
  String8 tag = {0};
  tag.size = count;
  tag.str = push_array_no_zero(arena, U8, count);
  tag.str = token.string.str;
  String8 split = str8_lit(" </>");
  String8List tag_split_open = str8_split_by_string_chars(arena, tag, split, StringSplitFlag_None);
  
  for(U32 idx = 0; idx < ArrayCount(html_encoding_table); ++idx)
  {
    B32 is_match = 0;
    HTMLTagEncoding html_tag_encoding = html_encoding_table[idx];
    for(String8Node *n = tag_split_open.first; n != 0; n = n->next)
    {
      if(str8_match(n->string, html_tag_encoding.tag_name, 0))
      {
        encoding = html_tag_encoding;
        is_match = 1;
        break;
      }
    }
    if(is_match) break;
  }
  
  if(encoding.type == HTMLTag_None)
  {
    parser->error.type = HTMLErrorType_unexpected_token;
    parser->error.at = parser->at;
  }
  switch(encoding.closing_type)
  {
    case HTMLTagClosingType_Paired:
    {
      do {
        token = html_get_token(parser);
      } while(token.type != HTMLTokenType_angle_bracket_close);
    } break;
    
    case HTMLTagClosingType_Self:
    {
      do {
        token = html_get_token(parser);
      } while(token.type != HTMLTokenType_angle_slash_then_bracket_close);
      result.range[1] = r1u64(parser->at-token.string.size, parser->at);
    } break;
    
    case HTMLTagClosingType_Unique:
    {
      do {
        token = html_get_token(parser);
      // } while(token.type == HTMLTokenType_dummy || token.type == HTMLTokenType_whitespace);
      } while(token.type != HTMLTokenType_angle_bracket_close);
      result.range[1] = r1u64(parser->at-token.string.size, parser->at);
    } break;
    default: 
    {
    } break;
  }
  range.max = range.min + tag.size + 1;
  if(result.range[0].max == 0)
  {
    result.range[0] = range;
  }
  else
  {
    result.range[1] = range;
  }
  result.encoding = encoding;
  return result;
}

internal HTMLElement *
html_parse_element_paired(Arena *arena, HTMLParser *parser, HTMLTag *end);
internal HTMLElement *
html_parse_element(Arena *arena, HTMLParser *parser, HTMLTag open_tag)
{
  HTMLElement *result = push_array_no_zero(arena, HTMLElement, 1);
  HTMLTag tag = open_tag;
  
  switch(open_tag.encoding.closing_type)
  {
    case HTMLTagClosingType_Paired:
    {
      result->next_sibbling = html_parse_element_paired(arena, parser, &tag);
    } break;
    case HTMLTagClosingType_Unique:
    {
    } break;
    case HTMLTagClosingType_Self:
    {
    } break;
    default:
    {
    } break;
  }
  do {
    ++parser->at;
  } while(char_is_whitespace(parser->string.str[parser->at]));
  
  result->tag = tag;
  return result;
}

internal HTMLElement *
html_parse_element_paired(Arena *arena, HTMLParser *parser, HTMLTag *tag)
{
  HTMLElement *last_child = {0};
  HTMLElement *first_child = {0};
  for(;html_is_parsing(parser);) 
  {
    HTMLTag next_tag = html_get_next_tag(arena, parser);
    if(next_tag.encoding.type == tag->encoding.type)
    {
      break;
    }
    HTMLElement *element = html_parse_element(arena, parser, next_tag);
    if(element)
    {
      /*
        TODO: why it does not work in cpp, it should
        last_child = (last_child ? last_child->next_sibbling : first_child) = ;
      */ 
      if(last_child)
      {
        last_child->next_sibbling = element;
      }
      else
      {
        first_child = element;
      }
      last_child = element;  
    }
  }
  return first_child;
}

internal String8
html_get_error_msg(Arena *arena, HTMLParser *parser, String8 file_path)
{
  String8List list = {0};
  str8_list_push(arena, &list, str8_lit("file path:"));
  str8_list_push(arena, &list, file_path);
  str8_list_push(arena, &list, str8_lit("Unexpected token:"));
  str8_list_push(arena, &list, str8_lit("at byte:"));
  str8_list_push(arena, &list, str8_from_u64(arena, parser->error.at, 10, 7, 0));
  str8_list_push(arena, &list, str8_lit("Error type:"));
  
  switch(parser->error.type)
  {
    case HTMLErrorType_unexpected_token: 
    {
      str8_list_push(arena, &list, str8_lit("Unexpected token:"));
    } break;
    
    default: 
    {
      str8_list_push(arena, &list, str8_lit("result"));
    } break; 
  }  
  String8 result = str8_list_join(arena, &list, str8_lit("\n"));
  return result;
}

internal String8
html_parse(Arena *arena, OS_FileInfoList *info_list)
{
  String8 result = {0};
  String8List *error_messages = push_array(arena, String8List, 1);
  for(OS_FileInfoNode *node = info_list->first;
      node != 0;
      node = node->next) 
  {
    StringMatchFlags match_flags = StringMatchFlag_CaseInsensitive;
    if(!str8_match(node->info.props.extension, str8_lit("html"), match_flags)) continue;

    U64 size_file = node->info.props.size;
    String8 file_name = push_str8_cat(arena, node->info.root_path, node->info.name);
    
    OS_Handle handle = os_file_open(arena, file_name, OS_AccessFlag_Read);
    if(os_handle_match(handle, os_handle_zero())) continue;

    U8 *memory = push_array_no_zero(arena, U8, size_file);
    U64 size = os_file_read(handle, rng_1u64(0, size_file), memory);
    
    HTMLParser parser = {0};
    parser.string.str = memory;
    parser.string.size = size;
    
    HTMLElementList *token_list = push_array(arena, HTMLElementList, 1);
    while(html_is_parsing(&parser))
    {
      HTMLTag tag = html_get_next_tag(arena, &parser);
      HTMLElement *element  = html_parse_element(arena, &parser, tag);
      html_token_push(arena, token_list, *element);    
      if(parser.error.type == HTMLErrorType_unexpected_token)
      {
        String8 error_msg = html_get_error_msg(arena, &parser, file_name);
        str8_list_push(arena, error_messages, error_msg);
        // break;
      }
      
  
    }
    result = str8_list_join(arena, error_messages, str8_lit("\n"));
  } 
  return result;     
}