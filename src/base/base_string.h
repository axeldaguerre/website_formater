#ifndef BASE_STRING_H
#define BASE_STRING_H

///////////////////////////////
// ~ Axel: String Types
typedef struct String8 String8;
struct String8 
{
  U8 *str;
  U64 size;
};

typedef struct String16 String16;
struct String16 
{
  U16 *str;
  U64 size;
};

///////////////////////////////
// ~ Axel: Unicode 

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
  U32 codepoint;
  U32 inc;
};

///////////////////////////////
// ~ Axel: List & Array Types 

typedef struct String8Node String8Node;
struct String8Node
{
  String8Node *next;
  String8     string;
};

typedef struct String8List String8List;
struct String8List
{
  String8Node *first;
  String8Node *last;
  U64         count;
  U64         total_size;
};

typedef struct String8Array String8Array;
struct String8Array
{
  String8 *strings;
  U64     count;
};
////////////////////////////////
//~ Axel: String Matching, Splitting, & Joining Types

typedef struct StringJoin StringJoin;
struct StringJoin
{
  String8 pre;
  String8 sep;
  String8 post;
};

typedef U32 StringMatchFlags;
enum
{
  StringMatchFlag_CaseInsensitive
};

internal String16      str16(U16 *str, U64 size);
internal U64           cstr16_length(U16 *str);
internal String16      str16_from_str8(Arena *arena, String8 str);

internal String8       str8(U8 *str, U64 size);
internal U64           cstr8_length(U8 *str);
internal String8       str8_from_16(Arena *arena, String16 str);
internal U32           utf8_encode(U8 *str, U32 codepoint);
internal String8       push_str8_copy(Arena *arena, String8 string);
internal String8       push_str8_cat(Arena *arena, String8 string, String8 cat);
internal String8Node*  str8_push_list(Arena *arena, String8List *list, String8 str);
internal String8       str8_join_from_list(Arena *arena, String8List *list);
internal B32           str8_ends_with(String8 str, String8 match);
internal B32           str8_match(String8 a, String8 b, StringMatchFlags);
internal String8       str8_post_fix(String8 str, U64 size);
internal String8       str8_from_16(Arena *arena, String16 utf16);
internal String8       str8_chop_last_slash(String8 str);
internal B32           str8_contains(String8 string, String8 pattern, StringMatchFlags flags);
internal String8       str8_join(Arena *arena, String8 a, String8 b);

#define str8_lit(S) str8((U8*)(S), sizeof(S) - 1)

#endif  