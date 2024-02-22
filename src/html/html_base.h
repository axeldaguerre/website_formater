#ifndef PARSER_BASE_H
#define PARSER_BASE_H

typedef U32 HTMLTagType;
enum
{
  HTMLTagType_Unknown,

  HTMLTagType_h1,
  HTMLTagType_h2,
  HTMLTagType_h3,
  HTMLTagType_h4,
    
  HTMLTagType_Error,
  
  HTMLTagType_Count,
};

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  HTMLTagType tag;
  String8     text;
  U64         byte_count;
};

typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  U8 *memory;
  U64 at;
  B32 on_error;
  U64 total_bytes;
};

typedef struct HTMLTokenNode HTMLTokenNode;
struct HTMLTokenNode
{
  HTMLTokenNode *next;
  HTMLToken     token;
};

typedef struct HTMLTokenList HTMLTokenList;
struct HTMLTokenList
{
  HTMLTokenNode *first;
  HTMLTokenNode *last;
  U64           count;
};


internal HTMLTokenNode* html_token_push_list(Arena *arena, HTMLTokenList *list, HTMLToken token);
internal HTMLToken      html_get_token_next(Arena *arena, HTMLParser *parser, String8 *contents);

internal B32            html_is_parsing(HTMLParser *parser);
internal B32            html_is_in_bounds(HTMLParser *parser);

internal void           html_parse(Arena *arena, OS_FileInfoList *list);

#endif