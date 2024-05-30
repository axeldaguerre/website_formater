#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#define STB_SPRINTF_DECORATE(name) ts_stbsp_##name
#include "third_party/ts_stb_sprintf.h"

////////////////////////////////
//~ rjf: Basic String Types

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

typedef struct String32 String32;
struct String32
{
 U32 *str;
 U64 size;
};

////////////////////////////////
//~ rjf: String List And Array Types

typedef struct String8Node String8Node;
struct String8Node
{
 String8Node *next;
 String8 string;
};

typedef struct String8List String8List;
struct String8List
{
 String8Node *first;
 String8Node *last;
 U64 node_count;
 U64 total_size;
};

typedef struct String8Array String8Array;
struct String8Array
{
 U64 count;
 String8 *v;
};

////////////////////////////////
//~ rjf: String Operation Types

typedef struct StringJoin StringJoin;
struct StringJoin
{
 String8 pre;
 String8 sep;
 String8 post;
};

typedef U32 MatchFlags;
enum
{
 MatchFlag_CaseInsensitive  = (1<<0),
 MatchFlag_RightSideSloppy  = (1<<1),
 MatchFlag_SlashInsensitive = (1<<2),
 MatchFlag_FindLast         = (1<<3),
 MatchFlag_KeepEmpties      = (1<<4),
};

typedef struct DecodedCodepoint DecodedCodepoint;
struct DecodedCodepoint
{
 U32 codepoint;
 U32 advance;
};

typedef enum IdentifierStyle
{
 IdentifierStyle_UpperCamelCase,
 IdentifierStyle_LowerCamelCase,
 IdentifierStyle_UpperCase,
 IdentifierStyle_LowerCase,
}
IdentifierStyle;

typedef enum PathKind
{
 PathKind_Relative,
 PathKind_DriveAbsolute,
 PathKind_RootAbsolute,
 PathKind_COUNT
}
PathKind;

////////////////////////////////
//~ rjf: Fuzzy Match Types

typedef struct FuzzyMatch FuzzyMatch;
struct FuzzyMatch
{
 Rng1U64 range_in_haystack;
};

typedef struct FuzzyMatchNode FuzzyMatchNode;
struct FuzzyMatchNode
{
 FuzzyMatchNode *next;
 FuzzyMatch match;
};

typedef struct FuzzyMatchList FuzzyMatchList;
struct FuzzyMatchList
{
 FuzzyMatchNode *first;
 FuzzyMatchNode *last;
 U64 count;
 U64 missed_count;
};

////////////////////////////////
//~ rjf: Char Functions
internal B32 CharIsWhitespace(U8 c);
internal B32 CharIsAlpha(U8 c);
internal B32 CharIsAlphaUpper(U8 c);
internal B32 CharIsAlphaLower(U8 c);
internal B32 CharIsDigit(U8 c);
internal B32 CharIsSymbol(U8 c);
internal B32 CharIsSpace(U8 c);
internal U8  CharToUpper(U8 c);
internal U8  CharToLower(U8 c);
internal U8  CharToForwardSlash(U8 c);

////////////////////////////////
//~ rjf: String Functions

//- rjf: Helpers
internal U64 CalculateCStringLength(char *cstr);

//- rjf: Basic Constructors
internal String8 Str8(U8 *str, U64 size);
#define Str8C(cstring) Str8((U8 *)(cstring), CalculateCStringLength(cstring))
#define Str8Lit(s) Str8((U8 *)(s), sizeof(s)-1)
#define Str8LitComp(s) {(U8 *)(s), sizeof(s)-1}
internal String8 Str8Range(U8 *first, U8 *one_past_last);
internal String16 Str16(U16 *str, U64 size);
internal String16 Str16C(U16 *ptr);
internal String32 Str32(U32 *str, U64 size);
#define Str8Struct(ptr) Str8((U8 *)(ptr), sizeof(*(ptr)))

//- rjf: Substrings
internal String8 Substr8(String8 str, Rng1U64 rng);
internal String8 Str8Skip(String8 str, U64 min);
internal String8 Str8Chop(String8 str, U64 nmax);
internal String8 Prefix8(String8 str, U64 size);
internal String8 Suffix8(String8 str, U64 size);

//- rjf: Matching
internal B32 Str8Match(String8 a, String8 b, MatchFlags flags);
internal U64 FindSubstr8(String8 haystack, String8 needle, U64 start_pos, MatchFlags flags);
internal FuzzyMatchList FindFuzzy8(Arena *arena, String8 haystack, String8 needle, U64 start_pos, MatchFlags flags);

//- rjf: Allocation
internal String8 PushStr8Copy(Arena *arena, String8 string);
internal String8 PushStr8FV(Arena *arena, char *fmt, va_list args);
internal String8 PushStr8F(Arena *arena, char *fmt, ...);
internal String8 PushStr8FillByte(Arena *arena, U64 size, U8 byte);

//- rjf: Use In Format Strings
#define Str8VArg(s) (int)(s).size, (s).str

//- rjf: String Lists
#define Str8ListFirst(list) ((list)->first != 0 ? (list)->first->string : Str8Lit(""))
internal void Str8ListPushNode(String8List *list, String8Node *n);
internal void Str8ListPushNodeFront(String8List *list, String8Node *n);
internal void Str8ListPush(Arena *arena, String8List *list, String8 str);
internal void Str8ListPushF(Arena *arena, String8List *list, char *fmt, ...);
internal void Str8ListPushFront(Arena *arena, String8List *list, String8 str);
internal void Str8ListConcatInPlace(String8List *list, String8List *to_push);
internal String8List StrSplit8(Arena *arena, String8 string, U64 split_count, String8 *splits);
internal String8 Str8ListJoin(Arena *arena, String8List list, StringJoin *optional_params);

//- rjf: String Re-Styling
internal String8 Str8Stylize(Arena *arena, String8 string, IdentifierStyle style, String8 separator);
internal String8 UpperFromStr8(Arena *arena, String8 string);
internal String8 LowerFromStr8(Arena *arena, String8 string);

////////////////////////////////
//~ rjf: Unicode Conversions

internal DecodedCodepoint DecodeCodepointFromUtf8(U8 *str, U64 max);
internal DecodedCodepoint DecodeCodepointFromUtf16(U16 *str, U64 max);
internal U32              Utf8FromCodepoint(U8 *out, U32 codepoint);
internal U32              Utf16FromCodepoint(U16 *out, U32 codepoint);
internal String8          Str8From16(Arena *arena, String16 str);
internal String16         Str16From8(Arena *arena, String8 str);
internal String8          Str8From32(Arena *arena, String32 str);
internal String32         Str32From8(Arena *arena, String8 str);

////////////////////////////////
//~ rjf: Skip/Chop Helpers

internal String8 Str8SkipWhitespace(String8 str);
internal String8 Str8ChopWhitespace(String8 str);
internal String8 Str8SkipChopWhitespace(String8 str);
internal String8 Str8SkipChopNewlines(String8 str);
internal String8 Str8SkipLastDot(String8 string);
internal String8 Str8ChopLastDot(String8 string);

////////////////////////////////
//~ rjf: Path Helpers

internal String8 Str8PathChopLastPeriod(String8 str);
internal String8 Str8PathSkipLastSlash(String8 str);
internal String8 Str8PathChopLastSlash(String8 str);
internal String8 Str8PathSkipLastPeriod(String8 str);
internal String8 Str8PathChopPastLastSlash(String8 str);

internal PathKind PathKindFromStr8(String8 path);
internal String8List PathPartsFromStr8(Arena *arena, String8 path);
internal String8List AbsolutePathPartsFromSourcePartsKind(Arena *arena, String8 source, String8List parts, PathKind kind);
internal String8List DotResolvedPathPartsFromParts(Arena *arena, String8List parts);
internal String8 NormalizedPathFromStr8(Arena *arena, String8 source, String8 path);

////////////////////////////////
//~ rjf: Array Functions

internal String8Array Str8ArrayFromList(Arena *arena, String8List list);

////////////////////////////////
//~ rjf: Numeric Conversions

internal U64 U64FromStr8(String8 str, U32 radix);
internal S64 CStyleIntFromStr8(String8 str);
internal F64 F64FromStr8(String8 str);
internal String8 CStyleHexStringFromU64(Arena *arena, U64 x, B32 caps);

////////////////////////////////
//~ rjf: Text Coordinates

internal TxtPt TxtPtFromStr8Off(String8 string, U64 off);

#endif // BASE_STRINGS_H
