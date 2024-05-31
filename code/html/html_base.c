internal HTMLElementAttributeNode *
HTML_AttributePush(Arena *arena, HTMLElementAttribute attribute, 
                    HTMLElementAttributeList *list)
{  
  HTMLElementAttributeNode *n = PushArray(arena, HTMLElementAttributeNode, 1);
  QueuePush(list->first, list->last, n);
  n->attribute.name = PushStr8Copy(arena, attribute.name);
  n->attribute.value = PushStr8Copy(arena, attribute.value);
  list->node_count++;
  
  return n;
}

internal B32  
HTML_ChildIsInsideParentTags(Arena *arena, HTMLElementNode *parent, HTMLElementNode *child)
{
  
  
  B32 result = 1;
  if(!parent || !child) return result;
  HTMLTag *parent_tag = parent->element.tags[0];
  HTMLTag *child_tag = child->element.tags[0];

  if(parent_tag->tag == HTMLTag_DIV)
  {
    return 1;
  }
  
  if(child_tag->flow_type == HTMLTagFlowContentType_Block)
  {
    // TODO: Having to add single paired here is dumb and prone to bugs
    if(parent_tag->tag == HTMLTag_H1       ||
       parent_tag->tag == HTMLTag_H2       ||
       parent_tag->tag == HTMLTag_H3       ||
       parent_tag->tag == HTMLTag_H4       ||
       parent_tag->tag == HTMLTag_H5       ||
       parent_tag->tag == HTMLTag_H6       ||
       parent_tag->tag == HTMLTag_A        ||
       parent_tag->tag == HTMLTag_B        ||
       parent_tag->tag == HTMLTag_BR       ||
       parent_tag->tag == HTMLTag_BUTTON   ||
       parent_tag->tag == HTMLTag_CODE     ||
       parent_tag->tag == HTMLTag_EM       ||
       parent_tag->tag == HTMLTag_IMG      ||
       parent_tag->tag == HTMLTag_INPUT    ||
       parent_tag->tag == HTMLTag_LABEL    ||
       parent_tag->tag == HTMLTag_SMALL    ||
       parent_tag->tag == HTMLTag_SPAN     ||
       parent_tag->tag == HTMLTag_STRONG   ||
       parent_tag->tag == HTMLTag_TEXTAREA ||
       parent_tag->tag == HTMLTag_U)
    {
      result = 0;
    }
  }
  
  return result;
}

internal void
HTML_TagFromInv(Arena *arena, HTMLTagInvariant *inv, HTMLTag *tag)
{
  if(inv)
  {
    tag->tag = inv->tag;
    tag->content_type = inv->content_type;
    tag->flow_type = inv->flow_type;
    tag->enclosing_type = inv->enclosing_type;
    tag->tag_name = inv->tag_name;
    tag->meaning = inv->meaning;
  }
}

internal HTMLParser *
HTML_InitParser(Arena *arena, String8 str, 
                 HTMLParserOutput *output)
{
  HTMLParser *parser = PushArray(arena, HTMLParser, 1);
  if(str.size)
  {
    parser->string = PushStr8Copy(arena, str);
    parser->string = str;
  }
  parser->error.messages = PushArray(arena, String8List, 1);  
  parser->output = output;
  
  return parser;
}

internal String8 
HTML_PrintErrorType(HTMLErrorType type)
{
  String8 result = {0};
  switch(type)
  {
    case HTMLErrorType_unexpected_token:
    {
      result = Str8Lit("unexpected token"); 
    } break;
    
    case HTMLErrorType_wrong_enclosing_type:
    {
      result = Str8Lit("wrong enclosing type"); 
    } break;
    
    default:
    {
      result = Str8Lit("Unknown error type"); 
    } break;
  }
  
  return result;
}

internal void
HTML_ParserSetError(Arena *arena, HTMLParser *parser, U64 at, 
                      HTMLErrorType type, String8 message)
{
  /*
    TODO: root_global arena is needed, we can loose the message later. As currently
          we're using only one arena pointer we're fine though
    TODO: having a global parser and parser's arena will permits to avoid heary HTML_ParserSetError()
  */ 
  String8 output = {0};
  output = PushStr8F(arena, "%S at byte: %S error type : %S", output, Str8FromU64(arena, at, 10, 2, 0), HTML_PrintErrorType(type));
  parser->error.at = at;
  parser->error.type |= type;
  Str8ListPush(arena, parser->error.messages, output);
}

internal B32 
HTML_TagIsClosing(HTMLTag *tag)
{
  B32 result = tag && tag->first_token.type != RawTokenType_angle_bracket_open;
  return result;
}

internal B32
HTML_IsInBounds(HTMLParser *parser)
{
  return (parser->at < parser->string.size);
}

internal B32
HTML_IsParsing(HTMLParser *parser)
{ 
  while(CharIsWhitespace(parser->string.str[parser->at]))
  {
    ++parser->at;
  }
  B32 result = parser->error.type == HTMLErrorType_Null && HTML_IsInBounds(parser);
  return result;
}

internal HTMLToken
HTML_GetTokenType(String8 string, U64 at)
{
  HTMLToken result = {0};
  result.range.min = at;
  result.range.max = at+1;
  
  U8 value = string.str[at];
  if(CharIsWhitespace(value))
  {
    result.type = RawTokenType_whitespace;
    return result;
  } 
  
  switch(value)
  {
    case '<': 
    {
      if(string.str[at+1] == '/')
      {
        ++result.range.max; 
        result.type = RawTokenType_angle_bracket_open_then_slash;
      }
      else if(string.str[at+1] == '!' && string.str[at+2] == '-' && string.str[at+3] == '-')
      {
        result.range.max = result.range.min + 4;
        result.type = RawTokenType_comment_open;
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
        ++result.range.max; 
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
    case '\"': 
    { 
      result.type = RawTokenType_double_quote;
    } break;
    case '\'': 
    { 
      result.type = RawTokenType_simple_quote;
    } break;
    case '=': 
    { 
      result.type = RawTokenType_equal;
    } break;
    case '&':
    { 
      // NOTE: lot of url's will trigger this. Youtube videos are plenty
      U64 at_end = result.range.min;
      while((at_end-result.range.min <= 4) && string.str[at_end] != ';')
      {
        ++at_end;
      }
      String8 escaped = Substr8(string, R1U64(result.range.min, at_end+1));
      if(Str8Match(Str8Lit("&amp;"), escaped, MatchFlag_CaseInsensitive) || // '&'
        Str8Match(Str8Lit("&lt;"), escaped, MatchFlag_CaseInsensitive)   || // '<'
        Str8Match(Str8Lit("&gt;"), escaped, MatchFlag_CaseInsensitive))     // '>'
      {
        result.type = RawTokenType_escaped_symbol;  
        result.range.max = result.range.min+escaped.size-1;
      }
      else 
      {
        result.type = RawTokenType_dummy;
      }
    } break;
    default:
    {
      if(string.str[at] == '-' && string.str[at+1] == '-' && string.str[at+2] == '>')
      {
        result.range.max = result.range.min + 3;
        result.type = RawTokenType_comment_close;
      }
       result.type = RawTokenType_dummy;
    } break;
  }
  
  return result;  
}

/*
  NOTE: We use only the first chunk of character to determine the tag, it permits to skip the 
        meaningfull tokens inside element's without complexify the computation. 
        Whitespace is computed, by consequence will fail.
*/
internal HTMLTagInvariant *
HTML_GetInvariantFromTagName(Arena *arena, String8 tag_name)
{  
    HTMLTagInvariant *result = {0};
    String8 splits[] = {Str8Lit(" "),  Str8Lit("\t"), 
                        Str8Lit("\r"), Str8Lit("\n")};

    String8List find_tag = StrSplit8(arena, tag_name, ArrayCount(splits), splits);
    
    HTMLTagInvariantTable table = HTML_GetTagTable();
    if(find_tag.total_size)
    {
      String8 string = find_tag.first->string;    
      B32 is_match = 0;
      for(U8 tag_inv_idx = 0; 
          tag_inv_idx < table.tag_count;
          ++tag_inv_idx)
      {
        HTMLTagInvariant *tag_inv = &table.tags[tag_inv_idx];
        if(Str8Match(string, tag_inv->tag_name, 0))
        {
          is_match = 1;
          result = tag_inv;
          break;
        }
        if(is_match) break;
      }
    }
    
    return result;
}

/*
  NOTE: In html, you will need to parse *any* meaningfull token for parsing before knowing if it's a dummy char or not
        Like having '<' could be dummy OR it could be the start of the html tag. Spec lack efficiency in that regard.
        We currently avoid the complexity and time wasted by asking to escape all meaningfull tokens in the element's content.
*/
internal HTMLToken
HTML_NextToken(HTMLParser *parser)
{
  HTMLToken result = {0};    
  RawTokenType  match = RawTokenType_angle_bracket_open            |
                        RawTokenType_angle_bracket_close           |
                        RawTokenType_angle_bracket_open_then_slash |
                        RawTokenType_angle_slash_then_bracket_close;
  
  U64 at = parser->at;
  
  while(HTML_IsParsing(parser) && !(result.type & match))
  {
    // TODO: the fact that HTML_IsParsing is not checking the actual at is problematic, found a bug when a pre was not closed in C:/Users/axeld/Documents/Projects/Projects_Repositories/axeldaguerre/public/Learn/Learn_Programming/Learn_Programming_Posts/Learn_Programming_IDK_1.html
    result = HTML_GetTokenType(parser->string, at);
    Assert(result.range.max > result.range.min);
    at += result.range.max-result.range.min;
  }
  
  parser->at = at;
  
  return result;
}

internal HTMLTag *
HTML_EatTag(Arena *arena, HTMLParser *parser)
{
  HTMLTag *tag = PushArrayNoZero(arena, HTMLTag, 1);
  
  for(;;)
  {
    tag->first_token = HTML_NextToken(parser);
    if(tag->first_token.type == RawTokenType_comment_open)
    {
      tag->last_token = HTML_NextToken(parser);
      if(tag->last_token.type == RawTokenType_comment_close)
      {
        HTML_ParserSetError(
          arena, parser, 
          tag->first_token.range.min,
          HTMLErrorType_unexpected_token, 
          Str8Lit("no closing comment tag"));
      }
      continue;
    }
    tag->last_token = HTML_NextToken(parser);
    
    U64 str_end = ClampTop(parser->string.size, tag->last_token.range.min);
    U64 str_start = ClampTop(str_end, tag->first_token.range.max);
    String8 tag_name = Str8((parser->string.str + str_start), str_end-str_start);
    HTMLTagInvariant *inv = HTML_GetInvariantFromTagName(arena, tag_name);
    HTML_TagFromInv(arena, inv, tag);
    
    if(tag && parser->skip_until_tag == HTMLTag_NULL) break;
    // NOTE: pre & code can have html meaningfull tokens, but are dummy
    else if(tag->tag == parser->skip_until_tag) break;
    // NOTE: first tag was dummy, but last tag could have been part of a meaningfull one
    parser->at = tag->first_token.range.max;
  }
  
  if((tag->tag & HTMLTag_PRE) || (tag->tag & HTMLTag_CODE))
  {
    if(!HTML_TagIsClosing(tag))
    {
      parser->skip_until_tag = tag->tag;
    }
    else
    {
      parser->skip_until_tag &= ~tag->tag;
    }
  }
  
  return tag;
}

internal HTMLElementAttributeList *
HTML_ParseAttributes(Arena *arena, HTMLParser *parser, HTMLElementNode *el_node)
{
  /*
     TODO: because we use simple while and the html parser function, as soon as we add another RawTokenType we 
            will break the computation.
  */
  HTMLElementAttributeList *list = PushArray(arena, HTMLElementAttributeList, 1);
  if(!el_node->element.tags[0]) return list;
  
  Rng1U64 rng = {0};
  rng.min = el_node->element.tags[0]->first_token.range.max + el_node->element.tags[0]->tag_name.size;
  rng.max = el_node->element.tags[0]->last_token.range.min;
  String8 string = Substr8(parser->string, rng);
  
  HTMLToken token = {0};  
  U64 at = 0;
  RawTokenType quotes = RawTokenType_simple_quote|RawTokenType_double_quote;
  
  while(at <= string.size)
  {
    /*
      NOTE: while parsing spec like html attributes, it result in nicer code when you delay the parsing the maximum. 
            Because that's always the next parsing that will tell you what was the previous one. 
            The parsing is forgiving because it's simple, we need more checks because html parser does not behave like this one.
            Like if the last pair/value is not a double quote, the attribute will still be parsed normally, 
            as the parser will know it's the end of the attribute's value when it enconter a non-dummy token. 
            In order to make it yell at us when it does, assert are here but TODO: we will need to write error msgs instead.
    */
    HTMLElementAttribute attribute = {0};
   
    token = HTML_GetTokenType(string, at++);
    
    if(token.type == RawTokenType_comment_open)
    {
      // TODO: really check the value "no-parsing-attributes", currently when we see comment symbol we skip. It's enough for now
      /* TODO: As we make error message along the way, we have to put the skip "no-parsing-attributes" at the beginning in order to avoid meaningless error msg,
              which is another clue that the error message must be made at the end or in parallel if possible
        */
      
      break;
    }
    else if(token.type == RawTokenType_dummy)
    {
      HTMLToken start = token;
      while(at <= string.size && token.type == RawTokenType_dummy)
      {
        token = HTML_GetTokenType(string, at++);
      }
      attribute.value = Str8(string.str + start.range.min, token.range.min-start.range.min);
      
      while(at <= string.size && token.type == RawTokenType_whitespace)
      {
        token = HTML_GetTokenType(string, at++);
      }
      
      if(token.type == RawTokenType_equal)
      {
        attribute.name = attribute.value;
        attribute.value = Str8Lit("");
        
        token = HTML_GetTokenType(string, at++);
        while(at <= string.size && token.type == RawTokenType_whitespace)
        {
          token = HTML_GetTokenType(string, at++);
        }
        HTMLToken first_quote = token;
        if(!(first_quote.type & quotes))
        {
            HTML_ParserSetError(arena, parser, 
              first_quote.range.min,
              HTMLErrorType_unexpected_token, 
              Str8Lit("tag's attribute value must be enclosed in quotes"));
        }
        
        token = HTML_GetTokenType(string, at++);
        if(token.type != RawTokenType_dummy)
        {          
          if(token.type & quotes)
          {
            HTML_ParserSetError(arena, parser, 
              token.range.min,
              HTMLErrorType_unexpected_token, 
              Str8Lit("tag's attribute value is empty"));
          }
        }
        
        start = token;
        
        while(at <= string.size && !(token.type & first_quote.type))
        {
          token = HTML_GetTokenType(string, at++);
        }
        
        HTMLToken last_quote = token;
        if(!(last_quote.type & quotes))
        {
             HTML_ParserSetError(arena, parser, 
              last_quote.range.min,
              HTMLErrorType_unexpected_token, 
              Str8Lit("couldn't find the last quotes from tag's attribute value"));
        }
        else if(last_quote.type != first_quote.type)
        {
          HTML_ParserSetError(arena, parser, 
              last_quote.range.min,
              HTMLErrorType_unexpected_token, 
              Str8Lit("tag's attribute value is enclosed by different quote types"));
        }
        
        attribute.value = Str8(string.str + start.range.min, token.range.min-start.range.min);
        token = HTML_GetTokenType(string, at++);        
      }
      HTML_AttributePush(arena, attribute, list);
    }          
  }
  return list;
}

internal String8
HTML_GetErrorMsg(Arena *arena, HTMLParser *parser)
{
  // TODO: add the warning to escape char like '<' == '&lt;' '>' = '&gt;' 
  String8 result = {0};
  if(parser->error.messages->node_count)
  {
    StringJoin join = {0};
    join.post = Str8Lit("\n");
    result = Str8ListJoin(arena, *parser->error.messages, &join);
  }
  return result;
}

internal HTMLElementNode 
HTML_ParseElement(Arena *arena, HTMLParser *parser, 
                  HTMLTag *first_tag, HTMLElementNode *root_n)
{
  HTMLElementNode el_node = {0};
  first_tag = el_node.element.tags[0] = !first_tag ? HTML_EatTag(arena, parser) : first_tag;
  if(first_tag->enclosing_type == HTMLTagEnclosingType_Paired)
  {
    HTMLTag *next_tag = {0};
    while(HTML_IsParsing(parser))
    {
      next_tag = HTML_EatTag(arena, parser);
      
      if(HTML_TagIsClosing(next_tag))
      {
        if(next_tag->tag == first_tag->tag)
        {
          el_node.element.tags[1] = next_tag;
          el_node.element.raw.data.str = parser->string.str + first_tag->last_token.range.max;
          el_node.element.raw.data.size = next_tag->first_token.range.min - first_tag->last_token.range.max;
        }   
        else
        {
          HTML_ParserSetError(arena, parser, 
                              first_tag->first_token.range.min,
                              HTMLErrorType_unexpected_token, 
                              Str8Lit("From opening tag, finding a closing tag from different type is not allowed"));
        }
        break;
      }
      else
      {
        parser->open_tag_count++;
        // TODO: Don't allocate if there is no child, point to a nil struct ?
        HTMLElementNode *first_child = PushArray(arena, HTMLElementNode, 1);
        *first_child = HTML_ParseElement(arena, parser, next_tag, root_n);
        DLLPushBack(el_node.first, el_node.last, first_child);
      }
    }
  }
  
  el_node.element.attributes = HTML_ParseAttributes(arena, parser, &el_node);
  parser->open_tag_count++;
  el_node.parent = root_n;
  
  return el_node;
}

internal HTMLElementNode 
HTML_ParseDocument(Arena *arena, HTMLParser *parser)
{
  HTMLElementNode root = {0};
  while(HTML_IsParsing(parser))
  {
    HTMLElementNode *node = PushArray(arena, HTMLElementNode, 1);
    *node = HTML_ParseElement(arena, parser, 0, &root);
    DLLPushBack(root.first, root.last, node);
  }
  return root;  
}

internal HTMLElementNode *
HTML_CreateElementFromTagType(Arena *arena, U64 type)
{
  HTMLElementNode *result = PushArray(arena, HTMLElementNode, 1);  
  HTMLTagInvariantTable table = HTML_GetTagTable();
  for(U32 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = &table.tags[idx];
    if(type == invariant->tag)
    {
      //TODO: Do the real computation (search this line to see other)
      result->element.tags[0] = PushArrayNoZero(arena, HTMLTag, 1);
      HTML_TagFromInv(arena, invariant, result->element.tags[0]);
      result->element.tags[1] = PushArrayNoZero(arena, HTMLTag, 1);
      HTML_TagFromInv(arena, invariant, result->element.tags[1]);      
      break;
    }
  }
  
  return result;
}

internal HTMLElementNode *
HTML_GetRootDocContentNode(HTMLElementNode *root)
{
  // TODO: will break one day
  HTMLElementNode *result = root->next->first->next->first;
  return result;
  
}


internal void
HTML_CreateAttributePush(Arena *arena, String8 name, String8 value, HTMLElement *el)
{
  HTMLElementAttribute *attribute = PushArray(arena, HTMLElementAttribute, 1);
  attribute->name = name;
  attribute->value = value;
  
  if(!el->attributes)
  {
    el->attributes = PushArray(arena, HTMLElementAttributeList, 1); 
  }
  HTML_AttributePush(arena, *attribute, el->attributes);
}

internal void
HTML_CreateAttribute(Arena *arena, String8 name, String8 value, HTMLElement *el)
{
  HTMLElementAttribute *attribute = PushArray(arena, HTMLElementAttribute, 1);
  attribute->name = name;
  attribute->value = value;
  
  if(!el->attributes)
  {
    el->attributes = PushArray(arena, HTMLElementAttributeList, 1); 
  }
  HTML_AttributePush(arena, *attribute, el->attributes);
}

/*
  NOTE: Return the element in which the content will have to be appends
*/
internal HTMLElementNode *
HTML_CreateRootDoc(Arena *arena, String8 doc_title)
{
  HTMLElementNode *root = PushArray(arena, HTMLElementNode, 1);
  
  HTMLElementNode *doctype = HTML_CreateElementFromTagType(arena, HTMLTag_DOCTYPE);
  DLLPushBack(root->first, root->last, doctype);
  doctype->root = root;
  HTML_CreateAttribute(arena, Str8Lit(""), Str8Lit("html"), &doctype->element);
  
  HTMLElementNode *html = HTML_CreateElementFromTagType(arena, HTMLTag_HTML);  
  DLLPushBack(root->first, root->last, html);
  html->root = root;
  HTML_CreateAttribute(arena, Str8Lit("lang"), Str8Lit("en"), &html->element);
  
  HTMLElementNode *head  = HTML_CreateElementFromTagType(arena, HTMLTag_HEAD);
  head->parent = html; 
  head->root = root;
  
  HTMLElementNode *body = HTML_CreateElementFromTagType(arena, HTMLTag_BODY);
  body->parent = html; 
  body->root = root;
  
  HTMLElementNode *div = HTML_CreateElementFromTagType(arena, HTMLTag_DIV);
  div->parent = body;  
  div->root = root;
  
  DLLPushBack(body->first, body->last, div);
  DLLPushBack(html->first, html->last, head);
  DLLPushBack(html->first, html->last, body);
  doctype->next = html;
  
  if(doc_title.size == 0) return doctype;
  HTMLElementNode *title = HTML_CreateElementFromTagType(arena, HTMLTag_TITLE);
  title->element.raw.data = PushStr8Copy(arena, doc_title);
  title->parent = head; 
  DLLPushBack(head->first, head->last, title);
  
  return div;
}

internal String8
HTML_Str8FromElementNoCheck(Arena *arena, HTMLParser *parser, HTMLElementNode *root)
{
  String8 result = {0};
  if(!root) return result;
  String8 el_str8 = {0};
  for(HTMLElementNode *n = root;
      n != 0;
      n = n->next)
  {
    HTMLTag *tag = n->element.tags[0];
    // NOTE: '§' is for indentation   
    el_str8 = PushStr8F(arena, "%S§<%S", el_str8, tag->tag_name);
    HTMLElementAttributeList *attributes = n->element.attributes;
    // TODO: avoid the check, constructor (function)?
    if(attributes)
    {
      for(HTMLElementAttributeNode *attribute_n = attributes->first;
          attribute_n != 0;
          attribute_n = attribute_n->next)
      {
          if(attribute_n->attribute.name.size !=0)
          {
            el_str8 = PushStr8F(arena, "%S%S=\"%S\"", el_str8, attribute_n->attribute.name, attribute_n->attribute.value); 
          }
          else
          {
            el_str8 = PushStr8F(arena, "%S%S", el_str8, attribute_n->attribute.value);
          }
      }
    }    
    switch(tag->enclosing_type)
    {
      case HTMLTagEnclosingType_Paired:
      {
        el_str8 = PushStr8F(arena,  "%S>%S", el_str8, n->element.raw.data);
        
        B32 are_childrens_inside = HTML_ChildIsInsideParentTags(arena, n, n->first);        
        String8 children_el_str8 = HTML_Str8FromElementNoCheck(arena, parser, n->first);
        if(!are_childrens_inside)
        {
          parser->open_tag_count++;
          el_str8 = PushStr8F(arena, "%S</%S>", el_str8, tag->tag_name); 
        }
        if(children_el_str8.size)
        {
          el_str8 = PushStr8F(arena, "%S\n§%S", el_str8, children_el_str8);
        }
        if(are_childrens_inside)
        {
          el_str8 = PushStr8F(arena, "%S</%S>", el_str8, tag->tag_name); 
          
        }
      } break;
      case HTMLTagEnclosingType_Self:
      {
        el_str8 = PushStr8F(arena, "%S/>", el_str8);
      } break;
      case HTMLTagEnclosingType_Unique:
      {
        el_str8 = PushStr8F(arena, "%S>", el_str8);
      } break;
    }
    //TODO: a internal exist for that Clamp, computation is failing anyway
    if(parser->open_tag_count == 0)
    {
      parser->open_tag_count = 0;
    }
    else
    {
      --parser->open_tag_count;
    }
    
    el_str8 = PushStr8F(arena, "%s\n§", el_str8);
  }
  StringJoin join = {0};
  String8 split = Str8Lit("§");
  String8List list_result = StrSplit8(arena, el_str8, split.size, &split);
  result = Str8ListJoin(arena, list_result, &join);
  
  return result;
}

internal HTMLTagInvariant *
HTML_FindFirstTagFromMeaning(Arena *arena, RawMeaning meaning)
{
  HTMLTagInvariant *result = PushArray(arena, HTMLTagInvariant, 1);
  HTMLTagInvariantTable table = HTML_GetTagTable();  
  for(U8 idx = 0; idx < table.tag_count; ++idx)
  {
    HTMLTagInvariant *invariant = table.tags + idx;
    if(invariant->meaning.strenght == meaning.strenght && 
       invariant->meaning.semantic_flags & meaning.semantic_flags)
    {
      result = invariant;
      break;
    }
  }
  
  return result;
}

internal HTMLElementNode *
HTML_CreateElFromRaw(Arena *arena, RawData *raw)
{
  HTMLElementNode *result = PushArray(arena, HTMLElementNode, 1);
  HTMLTagInvariant *inv = HTML_FindFirstTagFromMeaning(arena, raw->meaning);
  HTMLTag *tag = PushArrayNoZero(arena, HTMLTag, 1);
  HTML_TagFromInv(arena, inv, tag);
  result->element.tags[0] = result->element.tags[1] = tag;
  result->element.raw.meaning = raw->meaning;
  result->element.raw.data = PushStr8Copy(arena, raw->data);
  
  return result;
}

internal HTMLElementNode *
HTML_AppendToDefaultDoc(Arena *arena, String8 doc_title, HTMLElementNode *el)
{
  HTMLElementNode *root_doc = HTML_CreateRootDoc(arena, doc_title);
  // TODO (URGENT): will break
  el->parent = root_doc->next->first->next->first; // div
  root_doc->next->first->next->first = el;         // el->parent = div
  
  return root_doc;
}

internal void
HTML_ElNodeFromRawNode(Arena *arena, RawDataNode *raw_node, HTMLElementNode *parent)
{    
    /*
      NOTE: RawData and HTMLElement differ in terms of relation.
            - RawData's next and first means relation in their meaning
            - HTMElement's next and first means between the parent tag or sibbling to them
    */
  for(RawDataNode *n = raw_node;
      !RAW_IsNil(n);
      n = n->next)
  {
    HTMLElementNode *el_node = HTML_CreateElFromRaw(arena, &n->raw);
    el_node->parent = parent;
    el_node->root = parent->root;
    HTML_ElNodeFromRawNode(arena, n->first, el_node);
    DLLPushBack(parent->first, parent->last, el_node);
    if(n->next && !n->next->parent) break; // NOTE: the next node is a root node, it's a new html doc
  }
}
