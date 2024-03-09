#ifndef PARSER_BASE_H
#define PARSER_BASE_H

// HTML Token
typedef U32 HTMLTokenType;
enum
{
  HTMLTokenType_dummy = (1 << 0),
  HTMLTokenType_whitespace = (1 << 1),
  HTMLTokenType_angle_bracket_open = (1 << 2),
  HTMLTokenType_angle_bracket_close = (1 << 3),
  HTMLTokenType_angle_bracket_open_then_slash = (1 << 4),
  HTMLTokenType_angle_slash_then_bracket_close = (1 << 5),
  HTMLTokenType_slash = (1 << 6),
  
  HTMLTokenType_Error = (1 << 30),
  HTMLTokenType_Count = (1 << 31),
};

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  HTMLTokenType type;
  String8       string;
};

// HTML Element
typedef U32 HTMLTagType;
enum
{
  HTMLTag_None,

#define TAG(Tag, ...) HTMLTag_##Tag,
#include "html_tag_table.inl"
  
  HTMLTag_Count,
};

typedef U32 HTMLTagClosingType;
enum
{  
  HTMLTagClosingType_Paired,
  HTMLTagClosingType_Unique,
  HTMLTagClosingType_Self, 
};

typedef struct HTMLTagEncoding HTMLTagEncoding;
struct HTMLTagEncoding
{
  HTMLTagType            type;
  HTMLTagClosingType closing_type;
  String8            tag_name;
};

HTMLTagEncoding html_encoding_table[] =
{
 #include "html_tag_table.inl"
};

#endif
typedef struct HTMLTag HTMLTag;
struct HTMLTag
{
  HTMLTagEncoding encoding;
  Rng1U64 range[2];
};

typedef struct HTMLElement HTMLElement;
struct HTMLElement
{
  HTMLElement     *next_sibbling;
  HTMLTag         tag;
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
// Logging
typedef U32 HTMLErrorType;
enum
{
  HTMLErrorType_Null,
  
  HTMLErrorType_unexpected_token,
    
};


typedef struct HTMLError HTMLError;
struct HTMLError
{
  HTMLErrorType type;
  U64           at;
};

// Parser
typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  //TODO: saving the current token ? could get ride of multiple lines
  String8            string;
  HTMLError          error;
  U64                at;
};



