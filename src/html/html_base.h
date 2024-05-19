#ifndef HTML_BASE_H
#define HTML_BASE_H

// TODO: better names
typedef U32 RawTokenType;
enum
{
  RawTokenType_null                           = 0,
  RawTokenType_whitespace                     = (1 << 0),
  RawTokenType_angle_bracket_open             = (1 << 1),
  RawTokenType_double_quote                   = (1 << 2),
  RawTokenType_simple_quote                   = (1 << 3),
  RawTokenType_angle_bracket_close            = (1 << 4),
  RawTokenType_angle_bracket_open_then_slash  = (1 << 5),
  RawTokenType_angle_slash_then_bracket_close = (1 << 6),
  RawTokenType_slash                          = (1 << 7),
  RawTokenType_equal                          = (1 << 8),
  RawTokenType_dummy                          = (1 << 9),
};

typedef struct HTMLToken HTMLToken;
struct HTMLToken
{
  RawTokenType type;
  Rng1U64      range;
};

typedef struct HTMLElementAttribute HTMLElementAttribute;
struct HTMLElementAttribute
{
  union
  {
    struct
    {
      String8 name;  
      String8 value;
    };
    String8 pair[2];
  };
};

typedef struct HTMLElementAttributeNode HTMLElementAttributeNode;
struct HTMLElementAttributeNode
{
  HTMLElementAttributeNode *next;
  HTMLElementAttribute      attribute;
};

typedef struct HTMLElementAttributeList HTMLElementAttributeList;
struct HTMLElementAttributeList
{
  HTMLElementAttributeNode *first;
  HTMLElementAttributeNode *last;
  U64                       node_count;
};

typedef struct HTMLTag HTMLTag;
struct HTMLTag
{  
  U64                    tag;
  HTMLTagContentType     content_type;
  HTMLTagFlowContentType flow_type;
  HTMLTagEnclosingType   enclosing_type;
  HTMLToken              first_token;
  HTMLToken              last_token;
  String8                tag_name;
  RawMeaning             meaning;
};

typedef struct HTMLElement HTMLElement;
struct HTMLElement
{
  HTMLTag     *tags[2];
  RawData      raw;
  HTMLElementAttributeList *attributes;
};

typedef struct HTMLElementNode HTMLElementNode;
struct HTMLElementNode
{
  HTMLElementNode *root;
  HTMLElementNode *parent;
  HTMLElementNode *first;
  HTMLElementNode *last;
  HTMLElementNode *next;
  HTMLElementNode *prev; 
    
  HTMLElementNode *hash_next;
  HTMLElementNode *hash_prev;
  
  HTMLElement      element;
};

read_only global HTMLElementNode html_el_n_g_nil =
{
  &html_el_n_g_nil,
  &html_el_n_g_nil,
  &html_el_n_g_nil,
  &html_el_n_g_nil,
  &html_el_n_g_nil,
};

typedef struct HTMLElementList HTMLElementList;
struct HTMLElementList
{
  HTMLElementNode *first;
  HTMLElementNode *last;
  U64  node_count;
};

typedef U32 HTMLErrorType;
enum
{
  HTMLErrorType_Null                 = 0,
  HTMLErrorType_unexpected_token     = (1 << 0),
  HTMLErrorType_wrong_enclosing_type = (1 << 1),
  HTMLErrorType_wrong_flow_type      = (1 << 2),
  HTMLErrorType_wrong_content_type   = (1 << 3),
};


typedef struct HTMLError HTMLError;
struct HTMLError
{
  // TODO: Thanks to the messages push, we should may be able to delete others
  HTMLErrorType type;
  String8List  *messages;
  U64 at;
};

typedef struct HTMLParserOutput HTMLParserOutput;
struct HTMLParserOutput
{
  // String8   indent_ws;
  String8   indent_str;
  String8   indent_str_one;
  U8        space_by_indent;
  U64       max_text_width;
};

typedef struct HTMLParser HTMLParser;
struct HTMLParser
{
  String8           string;
  U64               skip_until_tag;
  HTMLError         error;
  HTMLParserOutput *output;
  U64               open_tag_count;
  U64               at;
};

typedef struct ReadingContent ReadingContent;
struct ReadingContent
{
  HTMLElementList html;
  String8          text;
};

#endif
