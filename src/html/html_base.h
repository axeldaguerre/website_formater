#ifndef PARSER_BASE_H
#define PARSER_BASE_H

typedef U32 HtmlTagType;
enum
{
  HtmlTagType_Unknown,

  HtmlTagType_h1,
  HtmlTagType_h2,
  HtmlTagType_h3,
  HtmlTagType_h4,

  HtmlTagType_Count
};

typedef struct HtmlToken HtmlToken;
struct HtmlToken
{
  HtmlTagType tag;
  String8     text;
  U64         byte_count;
};

typedef struct HtmlTokenNode HtmlTokenNode;
struct HtmlTokenNode
{
  HtmlTokenNode   *next;
  HtmlTagType tag;
};

typedef struct HtmlTokenList HtmlTokenList;
struct HtmlTokenList
{
  HtmlTokenNode *first;
  HtmlTokenNode *last;
  String8       filename;
  U64           count;
};

internal HtmlTokenNode* html_token_push_list(Arena *arena, HtmlTokenList *list, HtmlToken token);
internal void html_parse(Arena *arena, OS_FileInfoList *list);

#endif