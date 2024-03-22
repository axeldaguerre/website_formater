#ifndef HTML_BASE_H
#define HTML_BASE_H

typedef U32 RawTokenType;
enum
{
  RawTokenType_null                           = (1 << 0),
  
  RawTokenType_dummy                          = (1 << 1),
  RawTokenType_whitespace                     = (1 << 2),
  RawTokenType_angle_bracket_open             = (1 << 3),
  RawTokenType_angle_bracket_close            = (1 << 4),
  RawTokenType_angle_bracket_open_then_slash  = (1 << 5),
  RawTokenType_angle_slash_then_bracket_close = (1 << 6),
  RawTokenType_slash                          = (1 << 7),
  
  RawTokenType_Error                          = (1 << 30),
};

#define FIRST_TOKEN_TAG_FLAGS (RawTokenType_angle_bracket_open | RawTokenType_angle_bracket_open_then_slash)
#define LAST_TOKEN_TAG_FLAGS (RawTokenType_angle_slash_then_bracket_close | RawTokenType_angle_bracket_close)

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  RawTokenType type;
  String8      string;
};

typedef U32 HTMLTagType;
enum
{
  HTMLTag_None,

#define TAG(Tag, ...) HTMLTag_##Tag,
#include "html_tag_table.inl"
  
  HTMLTag_Count,
};

typedef U32 HTMLEnclosingType;
enum
{  
  HTMLEnclosingType_Null,
  HTMLEnclosingType_opening,
  HTMLEnclosingType_closing, 
};

typedef U32 HTMLTagEnclosingType;
enum
{  
  HTMLTagEnclosingType_Paired,
  HTMLTagEnclosingType_Unique,
  HTMLTagEnclosingType_Self, 
};

typedef struct HTMLTagEncoding HTMLTagEncoding;
struct HTMLTagEncoding
{
  HTMLTagType          type;
  HTMLTagEnclosingType enclosing_type;
  String8              tag_name;
  TextType             text_types[5]; // TODO: no constant size
};

internal HTMLTagEncoding html_encoding_table[] =
{
 #include "html_tag_table.inl"
};

internal HTMLTagEncoding
html_get_encoding_from_meaning(TextType type)
{
  HTMLTagEncoding result = {0};
  for(U32 idx = 0; idx < ArrayCount(html_encoding_table); ++idx)
  {
    HTMLTagEncoding encoding = html_encoding_table[idx];
    for(U8 text_type_idx = 0; 
      text_type_idx < ArrayCount(encoding.text_types); 
      ++text_type_idx)
      {
        if(encoding.text_types[text_type_idx] == type)
        {
          result = encoding;
          break;
        }
      }
    
  }
  return result;
}

typedef struct HTMLTag HTMLTag;
struct HTMLTag
{
  HTMLTagEncoding   encoding;
  HTMLEnclosingType enclosing_type;
  Rng1U64           range[2];
};

typedef struct HTMLElement HTMLElement;
struct HTMLElement
{
  HTMLElement *next_sibbling;
  HTMLTag      tag;
  String8      data;
};

typedef struct HTMLElementNode HTMLElementNode;
struct HTMLElementNode
{
  HTMLElementNode *next;
  HTMLElement      element;
};

typedef struct HTMLElementList HTMLElementList;
struct HTMLElementList
{
  HTMLElementNode *first;
  HTMLElementNode *last;
  U64           count;
};

typedef U32 HTMLErrorType;
enum
{
  HTMLErrorType_Null                 = (0),
  
  HTMLErrorType_unexpected_token     = (0 << 1),
  HTMLErrorType_wrong_enclosing_type = (1 << 1),
};


typedef struct HTMLError HTMLError;
struct HTMLError
{
  HTMLErrorType type;
  U64           at;
};

typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  String8   string;
  HTMLError error;
  U64       at;
};

typedef struct ReadingContent ReadingContent;
struct ReadingContent
{
  HTMLElementList html;
  String8          text;
};

#endif
