internal HTMLElementNode*
html_element_push(Arena *arena, HTMLElementList *list, HTMLElement element)
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

internal HTMLToken
html_get_token(String8 string, U64 at)
{
  HTMLToken result = {0};
  result.type = RawTokenType_null;
  result.string.str = string.str + at;
  result.string.size = 1;
  U8 value = string.str[at];
  switch(value)
  {
    case '<': 
    {
      if(string.str[at+1] == '/')
      {
        result.string.size = 2;
        result.type = RawTokenType_angle_bracket_open_then_slash;
      }
      else 
      {
        result.type = RawTokenType_angle_bracket_open;
      }
    } break;
    case '/': 
    {
      if(string.str[at+1] == '>')
      {
        result.string.size = 2;
        result.type = RawTokenType_angle_slash_then_bracket_close;
      }
      else 
      {
        result.type = RawTokenType_slash;
      }
      
    } break;
    case '>': 
    { 
      result.type = RawTokenType_angle_bracket_close;
    } break;      
    case ' ': 
    { 
      result.type = RawTokenType_whitespace;
    } break;
    default :
    {
      result.type = RawTokenType_dummy;
    } break;
  }  
  return result;
}

internal void 
html_validate_tag(HTMLParser *parser, HTMLTagEncoding encoding, String8 tag)
{
  if(!encoding.type)
  {
    parser->error.type |= HTMLErrorType_unexpected_token;
  }
  
  RawTokenType valide_last_token_type = {0};
  switch(encoding.enclosing_type)
  {
    case HTMLTagEnclosingType_Unique:
    case HTMLTagEnclosingType_Paired:
    {
      valide_last_token_type = RawTokenType_angle_bracket_close;
    } break;
    case HTMLTagEnclosingType_Self:
    {
      valide_last_token_type = RawTokenType_angle_slash_then_bracket_close;
    } break;
  }
  
  HTMLToken last_token = {0};
  U64 at = 0;
  
  while(at < tag.size && last_token.type != LAST_TOKEN_TAG_FLAGS)
  {
    last_token = html_get_token(tag, at);
    ++at;
  }
  
  if(last_token.type != valide_last_token_type)
  {
     parser->error.type |= HTMLErrorType_wrong_enclosing_type;
  }
  
  if(parser->error.type != HTMLErrorType_Null)
  {
    parser->error.at = parser->at;
  }
}

internal HTMLTag
html_get_next_tag(Arena *arena, HTMLParser *parser)
{
  HTMLToken token            = {0};
  HTMLTag result             = {0};
  Rng1U64 range              = {0};
  HTMLTagEncoding encoding   = {0};
  U64 at                     = parser->at;
  
  while(html_is_in_bounds(parser, at) && !(token.type & FIRST_TOKEN_TAG_FLAGS))
  {
     token = html_get_token(parser->string, at);
     at   += token.string.size;
  }
  if(token.type == RawTokenType_null) return result;
  range.min = at;
  
  while(html_is_in_bounds(parser, at) && !(token.type & LAST_TOKEN_TAG_FLAGS))
  {
     token = html_get_token(parser->string, at);
     at   += token.string.size;
  } 
  if(token.type == RawTokenType_null) return result;
  
  U64 size = at - range.min;
  range.max = range.min + size + 1;
  String8 tag = str8(parser->string.str + range.min, size);
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
  
  html_validate_tag(parser, encoding, tag);
  
  if(result.range[0].max == 0)
  {
    result.range[0] = range;
  }
  else
  {
    result.range[1] = range;
  }
  result.encoding = encoding;
  parser->at = at;
  return result;
}

internal HTMLElement *
html_parse_element_paired(Arena *arena, HTMLParser *parser, HTMLTag *end);
internal HTMLElement *
html_parse_element(Arena *arena, HTMLParser *parser)
{
  HTMLElement *result = push_array_no_zero(arena, HTMLElement, 1);
  HTMLTag opening_tag = html_get_next_tag(arena, parser);
  
  switch(opening_tag.encoding.enclosing_type)
  {
    case HTMLTagEnclosingType_Paired:
    {
      result->next_sibbling = html_parse_element_paired(arena, parser, &opening_tag);
      HTMLTag closing_tag = html_get_next_tag(arena, parser);
    } break;
    case HTMLTagEnclosingType_Unique:
    {
    } break;
    case HTMLTagEnclosingType_Self:
    {
    } break;    
  }
  
  result->tag = opening_tag;
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
    HTMLElement *element = html_parse_element(arena, parser);
    if(element)
    {      
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
  
  HTMLError error = parser->error;
  
  if(error.type & HTMLErrorType_unexpected_token)
  {
    str8_list_push(arena, &list, str8_lit("Unexpected token:\n"));
  }
  if(error.type & HTMLErrorType_wrong_enclosing_type)
  {
    str8_list_push(arena, &list, str8_lit("Wrong enclosing type:\n"));
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
    
    HTMLElementList *el_list = push_array(arena, HTMLElementList, 1);
    HTMLElement *first_el = {0};
    HTMLElement *last_el  = {0};
    while(html_is_parsing(&parser))
    {
      HTMLElement *el = html_parse_element(arena, &parser);
      if(!el) break;
      if(!first_el) first_el = el;
      if(last_el) last_el->next_sibbling = el;
      last_el = el;
      
      if(parser.error.type != HTMLErrorType_Null)
      {
        String8 error_msg = html_get_error_msg(arena, &parser, file_name);
        str8_list_push(arena, error_messages, error_msg);
      } 
    }
    if(first_el)
    {
      html_element_push(arena, el_list, *first_el);
    }
    result = str8_list_join(arena, error_messages, str8_lit("\n"));
  } 
  return result;     
}