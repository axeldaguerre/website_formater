#if BUILD_CORE
#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/ts_stb_sprintf.h"
#endif
////////////////////////////////
//~ NOTE(allen): String <-> Integer Tables

read_only global U8 integer_symbols[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

read_only global U8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
////////////////////////////////
////////////////////////////////
//~ rjf: Char Functions

internal B32
CharIsAlpha(U8 c)
{
 return CharIsAlphaUpper(c) || CharIsAlphaLower(c);
}

internal B32
CharIsAlphaUpper(U8 c)
{
 return c >= 'A' && c <= 'Z';
}

internal B32
CharIsAlphaLower(U8 c)
{
 return c >= 'a' && c <= 'z';
}

internal B32
CharIsDigit(U8 c)
{
 return (c >= '0' && c <= '9');
}

internal B32
CharIsWhitespace(U8 c)
{
  B32 result = 0;  
  result = ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  return result;
}

internal B32
CharIsSymbol(U8 c)
{
 return (c == '~' || c == '!'  || c == '$' || c == '%' || c == '^' ||
         c == '&' || c == '*'  || c == '-' || c == '=' || c == '+' ||
         c == '<' || c == '.'  || c == '>' || c == '/' || c == '?' ||
         c == '|' || c == '\\' || c == '{' || c == '}' || c == '(' ||
         c == ')' || c == '\\' || c == '[' || c == ']' || c == '#' ||
         c == ',' || c == ';'  || c == ':' || c == '@');
}

internal B32
CharIsSpace(U8 c)
{
 return c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v' || c == '\n';
}

internal U8
CharToUpper(U8 c)
{
 return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

internal U8
CharToLower(U8 c)
{
 return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'A')) : c;
}

internal U8
CharToForwardSlash(U8 c)
{
 return (c == '\\' ? '/' : c);
}

////////////////////////////////
//~ rjf: String Functions

//- rjf: Helpers

internal U64
CalculateCStringLength(char *cstr)
{
 U64 length = 0;
 for(;cstr[length]; length += 1);
 return length;
}

//- rjf: Basic Constructors

internal String8
Str8(U8 *str, U64 size)
{
 String8 string;
 string.str = str;
 string.size = size;
 return string;
}

internal String8
Str8Range(U8 *first, U8 *one_past_last)
{
 String8 string;
 string.str = first;
 string.size = (U64)(one_past_last - first);
 return string;
}

internal String16
Str16(U16 *str, U64 size)
{
 String16 result;
 result.str = str;
 result.size = size;
 return result;
}

internal String16
Str16C(U16 *ptr)
{
 U16 *p = ptr;
 for (;*p; p += 1);
 String16 result = Str16(ptr, p - ptr);
 return result;
}

internal String32
Str32(U32 *str, U64 size)
{
 String32 string = {0};
 string.str = str;
 string.size = size;
 return string;
}

//- rjf: Substrings

internal String8
Substr8(String8 str, Rng1U64 rng)
{
 U64 min = rng.min;
 U64 max = rng.max;
 if(max > str.size)
 {
  max = str.size;
 }
 if(min > str.size)
 {
  min = str.size;
 }
 if(min > max)
 {
  U64 swap = min;
  min = max;
  max = swap;
 }
 str.size = max - min;
 str.str += min;
 return str;
}

internal String8
Str8Skip(String8 str, U64 min)
{
 return Substr8(str, R1U64(min, str.size));
}

internal String8
Str8Chop(String8 str, U64 nmax)
{
 return Substr8(str, R1U64(0, str.size-nmax));
}

internal String8
Prefix8(String8 str, U64 size)
{
 return Substr8(str, R1U64(0, size));
}

internal String8
Suffix8(String8 str, U64 size)
{
 return Substr8(str, R1U64(str.size-size, str.size));
}

//- rjf: Matching

internal B32
Str8Match(String8 a, String8 b, MatchFlags flags)
{
 B32 result = 0;
 if(a.size == b.size || flags & MatchFlag_RightSideSloppy)
 {
  result = 1;
  for(U64 i = 0; i < a.size; i += 1)
  {
   B32 match = (a.str[i] == b.str[i]);
   if(flags & MatchFlag_CaseInsensitive)
   {
    match |= (CharToLower(a.str[i]) == CharToLower(b.str[i]));
   }
   if(flags & MatchFlag_SlashInsensitive)
   {
    match |= (CharToForwardSlash(a.str[i]) == CharToForwardSlash(b.str[i]));
   }
   if(match == 0)
   {
    result = 0;
    break;
   }
  }
 }
 return result;
}

internal U64
FindSubstr8(String8 haystack, String8 needle, U64 start_pos, MatchFlags flags)
{
 B32 found = 0;
 U64 found_idx = haystack.size;
 for(U64 i = start_pos; i < haystack.size; i += 1)
 {
  if(i + needle.size <= haystack.size)
  {
   String8 substr = Substr8(haystack, R1U64(i, i+needle.size));
   if(Str8Match(substr, needle, flags))
   {
    found_idx = i;
    found = 1;
    if(!(flags & MatchFlag_FindLast))
    {
     break;
    }
   }
  }
 }
 return found_idx;
}

internal FuzzyMatchList
FindFuzzy8(Arena *arena, String8 haystack, String8 needle, U64 start_pos, MatchFlags flags)
{
 Temp scratch = ScratchBegin(&arena, 1);
 FuzzyMatchList matches = {0};
 {
  String8 splits[] =
  {
   Str8Lit(" "),
   Str8Lit("/"),
   Str8Lit("\\"),
   Str8Lit("\t"),
   Str8Lit("\n"),
   Str8Lit("*"),
   Str8Lit("_"),
   Str8Lit("-"),
  };
  String8List needle_parts = StrSplit8(scratch.arena, needle, ArrayCount(splits), splits);
  U64 start_search_haystack_pos = 0;
  for(String8Node *n = needle_parts.first; n != 0; n = n->next)
  {
   B32 found = 0;
   for(U64 search_pos = start_search_haystack_pos; search_pos < haystack.size;)
   {
    U64 needle_part_pos = FindSubstr8(haystack, n->string, search_pos, flags);
    search_pos = needle_part_pos+1;
    if(needle_part_pos < haystack.size)
    {
     FuzzyMatchNode *match_n = PushArray(arena, FuzzyMatchNode, 1);
     match_n->match.range_in_haystack = R1U64(needle_part_pos, needle_part_pos + n->string.size);
     QueuePush(matches.first, matches.last, match_n);
     matches.count += 1;
     found = 1;
    }
   }
   if(found == 0)
   {
    matches.missed_count += 1;
   }
  }
 }
 ScratchEnd(scratch);
 return matches;
}

//- rjf: Allocation

internal String8
PushStr8Copy(Arena *arena, String8 string)
{
 String8 res;
 res.size = string.size;
 res.str = PushArrayNoZero(arena, U8, string.size + 1);
 MemoryCopy(res.str, string.str, string.size);
 res.str[string.size] = 0;
 return res;
}

internal String8
PushStr8FV(Arena *arena, char *fmt, va_list args)
{
 String8 result = {0};
 va_list args2;
 va_copy(args2, args);
 U64 needed_bytes = ts_stbsp_vsnprintf(0, 0, fmt, args)+1;
 result.str = PushArrayNoZero(arena, U8, needed_bytes);
 result.size = needed_bytes - 1;
 ts_stbsp_vsnprintf((char*)result.str, needed_bytes, fmt, args2);
 return result;
}

internal String8
PushStr8F(Arena *arena, char *fmt, ...)
{
 String8 result = {0};
 va_list args;
 va_start(args, fmt);
 result = PushStr8FV(arena, fmt, args);
 va_end(args);
 return result;
}

internal String8
PushStr8FillByte(Arena *arena, U64 size, U8 byte)
{
 String8 result = {0};
 result.str = PushArrayNoZero(arena, U8, size);
 MemorySet(result.str, byte, size);
 result.size = size;
 return result;
}

//- rjf: String Lists

internal void
Str8ListPushNode(String8List *list, String8Node *n)
{
 QueuePush(list->first, list->last, n);
 list->node_count += 1;
 list->total_size += n->string.size;
}

internal void
Str8ListPushNodeFront(String8List *list, String8Node *n)
{
 QueuePushFront(list->first, list->last, n);
 list->node_count += 1;
 list->total_size += n->string.size;
}

internal void
Str8ListPush(Arena *arena, String8List *list, String8 str)
{
 String8Node *n = PushArray(arena, String8Node, 1);
 n->string = str;
 Str8ListPushNode(list, n);
}

internal void
Str8ListPushF(Arena *arena, String8List *list, char *fmt, ...)
{
 va_list args;
 va_start(args, fmt);
 String8 string = PushStr8FV(arena, fmt, args);
 va_end(args);
 Str8ListPush(arena, list, string);
}

internal void
Str8ListPushFront(Arena *arena, String8List *list, String8 str)
{
 String8Node *n = PushArray(arena, String8Node, 1);
 n->string = str;
 Str8ListPushNodeFront(list, n);
}

internal void
Str8ListConcatInPlace(String8List *list, String8List *to_push)
{
 if(to_push->first)
 {
  list->node_count += to_push->node_count;
  list->total_size += to_push->total_size;
  if(list->last == 0)
  {
   *list = *to_push;
  }
  else
  {
   list->last->next = to_push->first;
   list->last = to_push->last;
  }
 }
 MemoryZero(to_push, sizeof(*to_push));
}

internal String8List
StrSplit8(Arena *arena, String8 string, U64 split_count, String8 *splits)
{
 String8List list = {0};
 
 U64 split_start = 0;
 for(U64 i = 0; i < string.size; i += 1)
 {
  B32 was_split = 0;
  for(U64 split_idx = 0; split_idx < split_count; split_idx += 1)
  {
   B32 match = 0;
   if(i + splits[split_idx].size <= string.size)
   {
    match = 1;
    for(U64 split_i = 0; split_i < splits[split_idx].size && i + split_i < string.size; split_i += 1)
    {
     if(splits[split_idx].str[split_i] != string.str[i + split_i])
     {
      match = 0;
      break;
     }
    }
   }
   if(match)
   {
    String8 split_string = Str8(string.str + split_start, i - split_start);
    Str8ListPush(arena, &list, split_string);
    split_start = i + splits[split_idx].size;
    i += splits[split_idx].size - 1;
    was_split = 1;
    break;
   }
  }
  
  if(was_split == 0 && i == string.size - 1)
  {
   String8 split_string = Str8(string.str + split_start, i+1 - split_start);
   Str8ListPush(arena, &list, split_string);
   break;
  }
 }
 
 return list;
}

internal String8
Str8ListJoin(Arena *arena, String8List list, StringJoin *optional_params)
{
 // rjf: setup join parameters
 StringJoin join = {0};
 if(optional_params != 0)
 {
  MemoryCopy(&join, optional_params, sizeof(join));
 }
 
 // rjf: calculate size & allocate
 U64 sep_count = 0;
 if(list.node_count > 1)
 {
  sep_count = list.node_count - 1;
 }
 String8 result = {0};
 result.size = (list.total_size + join.pre.size +
                sep_count*join.sep.size + join.post.size);
 result.str = PushArrayNoZero(arena, U8, result.size+1);
 
 // rjf: fill
 U8 *ptr = result.str;
 MemoryCopy(ptr, join.pre.str, join.pre.size);
 ptr += join.pre.size;
 for(String8Node *node = list.first; node; node = node->next)
 {
  MemoryCopy(ptr, node->string.str, node->string.size);
  ptr += node->string.size;
  if(node != list.last)
  {
   MemoryCopy(ptr, join.sep.str, join.sep.size);
   ptr += join.sep.size;
  }
 }
 MemoryCopy(ptr, join.post.str, join.post.size);
 ptr += join.post.size;
 
 // rjf: add null
 result.str[result.size] = 0;
 
 return result;
}

//- rjf: String Re-Styling

internal String8
Str8Stylize(Arena *arena, String8 string, IdentifierStyle style, String8 separator)
{
 String8 result = {0};
 
 String8List words = {0};
 
 B32 break_on_uppercase = 0;
 {
  break_on_uppercase = 1;
  for(U64 i = 0; i < string.size; i += 1)
  {
   if(!CharIsAlpha(string.str[i]) && !CharIsDigit(string.str[i]))
   {
    break_on_uppercase = 0;
    break;
   }
  }
 }
 
 B32 making_word = 0;
 String8 word = {0};
 
 for(U64 i = 0; i < string.size;)
 {
  if(making_word)
  {
   if((break_on_uppercase && CharIsAlphaUpper(string.str[i])) ||
      string.str[i] == '_' || CharIsSpace(string.str[i]) ||
      i == string.size - 1)
   {
    if(i == string.size - 1)
    {
     word.size += 1;
    }
    making_word = 0;
    Str8ListPush(arena, &words, word);
   }
   else
   {
    word.size += 1;
    i += 1;
   }
  }
  else
  {
   if(CharIsAlpha(string.str[i]))
   {
    making_word = 1;
    word.str = string.str + i;
    word.size = 1;
   }
   i += 1;
  }
 }
 
 result.size = words.total_size;
 if(words.node_count > 1)
 {
  result.size += separator.size*(words.node_count-1);
 }
 result.str = PushArrayNoZero(arena, U8, result.size);
 
 {
  U64 write_pos = 0;
  for(String8Node *node = words.first; node; node = node->next)
  {
   
   // NOTE(rjf): Write word string to result.
   {
    MemoryCopy(result.str + write_pos, node->string.str, node->string.size);
    
    // NOTE(rjf): Transform string based on word style.
    switch(style)
    {
     case IdentifierStyle_UpperCamelCase:
     {
      result.str[write_pos] = CharToUpper(result.str[write_pos]);
      for(U64 i = write_pos+1; i < write_pos + node->string.size; i += 1)
      {
       result.str[i] = CharToLower(result.str[i]);
      }
     }break;
     
     case IdentifierStyle_LowerCamelCase:
     {
      result.str[write_pos] = node == words.first ? CharToLower(result.str[write_pos]) : CharToUpper(result.str[write_pos]);
      for(U64 i = write_pos+1; i < write_pos + node->string.size; i += 1)
      {
       result.str[i] = CharToLower(result.str[i]);
      }
     }break;
     
     case IdentifierStyle_UpperCase:
     {
      for(U64 i = write_pos; i < write_pos + node->string.size; i += 1)
      {
       result.str[i] = CharToUpper(result.str[i]);
      }
     }break;
     
     case IdentifierStyle_LowerCase:
     {
      for(U64 i = write_pos; i < write_pos + node->string.size; i += 1)
      {
       result.str[i] = CharToLower(result.str[i]);
      }
     }break;
     
     default: break;
    }
    
    write_pos += node->string.size;
   }
   
   if(node->next)
   {
    MemoryCopy(result.str + write_pos, separator.str, separator.size);
    write_pos += separator.size;
   }
  }
 }
 
 return result;
}

internal String8
UpperFromStr8(Arena *arena, String8 string)
{
 String8 result = PushStr8Copy(arena, string);
 for(U64 idx = 0; idx < string.size; idx += 1)
 {
  result.str[idx] = CharToUpper(string.str[idx]);
 }
 return result;
}

internal String8
LowerFromStr8(Arena *arena, String8 string)
{
 String8 result = PushStr8Copy(arena, string);
 for(U64 idx = 0; idx < string.size; idx += 1)
 {
  result.str[idx] = CharToLower(string.str[idx]);
 }
 return result;
}

////////////////////////////////
//~ rjf: Unicode Conversions

read_only global U8 utf8_class[32] =
{
 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

#define bitmask1 0x01
#define bitmask2 0x03
#define bitmask3 0x07
#define bitmask4 0x0F
#define bitmask5 0x1F
#define bitmask6 0x3F
#define bitmask7 0x7F
#define bitmask8 0xFF
#define bitmask9  0x01FF
#define bitmask10 0x03FF

internal DecodedCodepoint
DecodeCodepointFromUtf8(U8 *str, U64 max)
{
 DecodedCodepoint result = {~((U32)0), 1};
 U8 byte = str[0];
 U8 byte_class = utf8_class[byte >> 3];
 switch (byte_class)
 {
  case 1:
  {
   result.codepoint = byte;
  }break;
  
  case 2:
  {
   if(2 <= max)
   {
    U8 cont_byte = str[1];
    if(utf8_class[cont_byte >> 3] == 0)
    {
     result.codepoint = (byte & bitmask5) << 6;
     result.codepoint |=  (cont_byte & bitmask6);
     result.advance = 2;
    }
   }
  }break;
  
  case 3:
  {
   if(3 <= max)
   {
    U8 cont_byte[2] = {str[1], str[2]};
    if(utf8_class[cont_byte[0] >> 3] == 0 &&
       utf8_class[cont_byte[1] >> 3] == 0)
    {
     result.codepoint = (byte & bitmask4) << 12;
     result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
     result.codepoint |=  (cont_byte[1] & bitmask6);
     result.advance = 3;
    }
   }
  }break;
  
  case 4:
  {
   if(4 <= max)
   {
    U8 cont_byte[3] = {str[1], str[2], str[3]};
    if(utf8_class[cont_byte[0] >> 3] == 0 &&
       utf8_class[cont_byte[1] >> 3] == 0 &&
       utf8_class[cont_byte[2] >> 3] == 0)
    {
     result.codepoint = (byte & bitmask3) << 18;
     result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
     result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
     result.codepoint |=  (cont_byte[2] & bitmask6);
     result.advance = 4;
    }
   }
  }break;
 }
 
 return result;
}

internal DecodedCodepoint
DecodeCodepointFromUtf16(U16 *out, U64 max)
{
 DecodedCodepoint result = {~((U32)0), 1};
 result.codepoint = out[0];
 result.advance = 1;
 if(1 < max && 0xD800 <= out[0] && out[0] < 0xDC00 && 0xDC00 <= out[1] && out[1] < 0xE000)
 {
  result.codepoint = ((out[0] - 0xD800) << 10) | (out[1] - 0xDC00) + 0x10000;
  result.advance = 2;
 }
 return result;
}

internal U32             
Utf8FromCodepoint(U8 *out, U32 codepoint)
{
#define bit8 0x80
 U32 advance = 0;
 if(codepoint <= 0x7F)
 {
  out[0] = (U8)codepoint;
  advance = 1;
 }
 else if(codepoint <= 0x7FF)
 {
  out[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
  out[1] = bit8 | (codepoint & bitmask6);
  advance = 2;
 }
 else if(codepoint <= 0xFFFF)
 {
  out[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
  out[1] = bit8 | ((codepoint >> 6) & bitmask6);
  out[2] = bit8 | ( codepoint       & bitmask6);
  advance = 3;
 }
 else if(codepoint <= 0x10FFFF)
 {
  out[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
  out[1] = bit8 | ((codepoint >> 12) & bitmask6);
  out[2] = bit8 | ((codepoint >>  6) & bitmask6);
  out[3] = bit8 | ( codepoint        & bitmask6);
  advance = 4;
 }
 else
 {
  out[0] = '?';
  advance = 1;
 }
 return advance;
}

internal U32             
Utf16FromCodepoint(U16 *out, U32 codepoint)
{
 U32 advance = 1;
 if(codepoint == ~((U32)0))
 {
  out[0] = (U16)'?';
 }
 else if(codepoint < 0x10000)
 {
  out[0] = (U16)codepoint;
 }
 else
 {
  U64 v = codepoint - 0x10000;
  out[0] = 0xD800 + (v >> 10);
  out[1] = 0xDC00 + (v & bitmask10);
  advance = 2;
 }
 return advance;
}

internal String8         
Str8From16(Arena *arena, String16 in)
{
 U64 cap = in.size*3;
 U8 *str = PushArrayNoZero(arena, U8, cap + 1);
 U16 *ptr = in.str;
 U16 *opl = ptr + in.size;
 U64 size = 0;
 DecodedCodepoint consume;
 for(;ptr < opl;)
 {
  consume = DecodeCodepointFromUtf16(ptr, opl - ptr);
  ptr += consume.advance;
  size += Utf8FromCodepoint(str + size, consume.codepoint);
 }
 str[size] = 0;
 ArenaPop(arena, cap - size); // := ((cap + 1) - (size + 1))
 return Str8(str, size);
}

internal String16        
Str16From8(Arena *arena, String8 in)
{
 U64 cap = in.size*2;
 U16 *str = PushArrayNoZero(arena, U16, cap + 1);
 U8 *ptr = in.str;
 U8 *opl = ptr + in.size;
 U64 size = 0;
 DecodedCodepoint consume;
 for(;ptr < opl;)
 {
  consume = DecodeCodepointFromUtf8(ptr, opl - ptr);
  ptr += consume.advance;
  size += Utf16FromCodepoint(str + size, consume.codepoint);
 }
 str[size] = 0;
 ArenaPop(arena, 2*(cap - size)); // := 2*((cap + 1) - (size + 1))
 String16 result = {str, size};
 return result;
}

internal String8         
Str8From32(Arena *arena, String32 in)
{
 U64 cap = in.size*4;
 U8 *str = PushArrayNoZero(arena, U8, cap + 1);
 U32 *ptr = in.str;
 U32 *opl = ptr + in.size;
 U64 size = 0;
 DecodedCodepoint consume;
 for(;ptr < opl; ptr += 1)
 {
  size += Utf8FromCodepoint(str + size, *ptr);
 }
 str[size] = 0;
 ArenaPop(arena, cap - size); // := ((cap + 1) - (size + 1))
 return Str8(str, size);
}

internal String8
Str8FromU64(Arena *arena, U64 u64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
  String8 result = {0};
  {
    // rjf: prefix
    String8 prefix = {0};
    switch(radix)
    {
      case 16:{prefix = Str8Lit("0x");}break;
      case 8: {prefix = Str8Lit("0o");}break;
      case 2: {prefix = Str8Lit("0b");}break;
    }
    
    // rjf: determine # of chars between separators
    U8 digit_group_size = 3;
    switch(radix)
    {
      default:break;
      case 2:
      case 8:
      case 16:
      {digit_group_size = 4;}break;
    }
    
    // rjf: prep
    U64 needed_leading_0s = 0;
    {
      U64 needed_digits = 1;
      {
        U64 u64_reduce = u64;
        for(;;)
        {
          u64_reduce /= radix;
          if(u64_reduce == 0)
          {
            break;
          }
          needed_digits += 1;
        }
      }
      needed_leading_0s = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
      U64 needed_separators = 0;
      if(digit_group_separator != 0)
      {
        needed_separators = (needed_digits+needed_leading_0s)/digit_group_size;
        if(needed_separators > 0 && (needed_digits+needed_leading_0s)%digit_group_size == 0)
        {
          needed_separators -= 1;
        }
      }
      result.size = prefix.size + needed_leading_0s + needed_separators + needed_digits;
      result.str = PushArrayNoZero(arena, U8, result.size + 1);
      result.str[result.size] = 0;
    }
    
    // rjf: fill contents
    {
      U64 u64_reduce = u64;
      U64 digits_until_separator = digit_group_size;
      for(U64 idx = 0; idx < result.size-prefix.size; idx += 1)
      {
        if(digits_until_separator == 0 && digit_group_separator != 0)
        {
          result.str[result.size - idx - 1] = digit_group_separator;
          digits_until_separator = digit_group_size+1;
        }
        else if(u64_reduce != 0)
        {
          result.str[result.size - idx - 1] = CharToLower(integer_symbols[u64_reduce%radix]);
          u64_reduce /= radix;
        }
        else
        {
          result.str[result.size - idx - 1] = '0';
        }
        digits_until_separator -= 1;        
      }      
    }
    
    // rjf: fill prefix
    if(prefix.size != 0)
    {
      MemoryCopy(result.str, prefix.str, prefix.size);
    }
  }
  return result;
}

internal String32        
Str32From8(Arena *arena, String8 in)
{
 U64 cap = in.size;
 U32 *str = PushArrayNoZero(arena, U32, cap + 1);
 U8 *ptr = in.str;
 U8 *opl = ptr + in.size;
 U64 size = 0;
 DecodedCodepoint consume;
 for(;ptr < opl;)
 {
  consume = DecodeCodepointFromUtf8(ptr, opl - ptr);
  ptr += consume.advance;
  str[size] = consume.codepoint;
  size += 1;
 }
 str[size] = 0;
 ArenaPop(arena, 4*(cap - size)); // := 4*((cap + 1) - (size + 1))
 String32 result = {str, size};
 return result;
}

////////////////////////////////
//~ rjf: Skip/Chop Helpers

internal String8
Str8SkipWhitespace(String8 str)
{
 U64 first_non_ws = 0;
 for(U64 idx = 0; idx < str.size; idx += 1)
 {
  first_non_ws = idx;
  if(!CharIsSpace(str.str[idx]))
  {
   break;
  }
  else if(idx == str.size-1)
  {
   first_non_ws = 1;
  }
 }
 return Substr8(str, R1U64(first_non_ws, str.size));
}

internal String8
Str8ChopWhitespace(String8 str)
{
 U64 first_ws_at_end = str.size;
 for(U64 idx = str.size-1; idx < str.size; idx -= 1)
 {
  if(!CharIsSpace(str.str[idx]))
  {
   break;
  }
  first_ws_at_end = idx;
 }
 return Substr8(str, R1U64(0, first_ws_at_end));
}

internal String8
Str8SkipChopWhitespace(String8 str)
{
 return Str8SkipWhitespace(Str8ChopWhitespace(str));
}

internal String8
Str8SkipChopNewlines(String8 str)
{
 U64 first_non_ws = 0;
 for(U64 idx = 0; idx < str.size; idx += 1)
 {
  first_non_ws = idx;
  if(str.str[idx] != '\n' && str.str[idx] != '\r')
  {
   break;
  }
 }
 
 U64 first_ws_at_end = str.size;
 for(U64 idx = str.size-1; idx < str.size; idx -= 1)
 {
  if(str.str[idx] != '\n' && str.str[idx] != '\r')
  {
   break;
  }
  first_ws_at_end = idx;
 }
 
 return Substr8(str, R1U64(first_non_ws, first_ws_at_end));
}

internal String8
Str8SkipLastDot(String8 string){
  String8 result = string;
  U64 p = string.size;
  for (;p > 0;){
    p -= 1;
    if (string.str[p] == '.'){
      result = Str8Skip(string, p + 1);
      break;
    }
  }
  return(result);
}

internal String8
Str8ChopLastDot(String8 string)
{
  String8 result = string;
  U64 p = string.size;
  for (;p > 0;){
    p -= 1;
    if (string.str[p] == '.'){
      result = Prefix8(string, p);
      break;
    }
  }
  return(result);
}

////////////////////////////////
//~ rjf: Path Helpers

internal String8
Str8PathChopLastPeriod(String8 string)
{
 U64 period_pos = FindSubstr8(string, Str8Lit("."), 0, MatchFlag_FindLast);
 if(period_pos < string.size)
 {
  string.size = period_pos;
 }
 return string;
}

internal String8
Str8PathSkipLastSlash(String8 string)
{
 U64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0,
                             MatchFlag_SlashInsensitive|
                             MatchFlag_FindLast);
 if(slash_pos < string.size)
 {
  string.str += slash_pos+1;
  string.size -= slash_pos+1;
 }
 return string;
}

internal String8
Str8PathChopLastSlash(String8 string)
{
 U64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0, MatchFlag_SlashInsensitive|MatchFlag_FindLast);
 if(slash_pos < string.size)
 {
  string.size = slash_pos;
 }
 return string;
}

internal String8
Str8PathSkipLastPeriod(String8 string)
{
 U64 period_pos = FindSubstr8(string, Str8Lit("."), 0, MatchFlag_FindLast);
 if(period_pos < string.size)
 {
  string.str += period_pos+1;
  string.size -= period_pos+1;
 }
 return string;
}

internal String8
Str8PathChopPastLastSlash(String8 string)
{
 U64 slash_pos = FindSubstr8(string, Str8Lit("/"), 0, MatchFlag_SlashInsensitive|MatchFlag_FindLast);
 if(slash_pos < string.size)
 {
  string.size = slash_pos+1;
 }
 return string;
}

internal PathKind
PathKindFromStr8(String8 path)
{
 PathKind kind = PathKind_Relative;
 if(path.size >= 1 && path.str[0] == '/')
 {
  kind = PathKind_RootAbsolute;
 }
 if(path.size >= 2 && CharIsAlpha(path.str[0]) && path.str[1] == ':')
 {
  kind = PathKind_DriveAbsolute;
 }
 return kind;
}

internal String8List
PathPartsFromStr8(Arena *arena, String8 path)
{
 String8 splits[] = {Str8Lit("/"), Str8Lit("\\")};
 String8List strs = StrSplit8(arena, path, ArrayCount(splits), splits);
 return strs;
}

internal String8List
AbsolutePathPartsFromSourcePartsKind(Arena *arena, String8 source, String8List parts, PathKind kind)
{
 if(kind == PathKind_Relative)
 {
  String8List concatted_parts = {0};
  String8List source_parts = PathPartsFromStr8(arena, source);
  Str8ListConcatInPlace(&concatted_parts, &source_parts);
  Str8ListConcatInPlace(&concatted_parts, &parts);
  parts = concatted_parts;
 }
 return parts;
}

internal String8List
DotResolvedPathPartsFromParts(Arena *arena, String8List parts)
{
 Temp scratch = ScratchBegin(&arena, 1);
 typedef struct NodeNode NodeNode;
 struct NodeNode
 {
  NodeNode *next;
  String8Node *node;
 };
 NodeNode *part_stack_top = 0;
 for(String8Node *n = parts.first; n != 0; n = n->next)
 {
  if(Str8Match(n->string, Str8Lit(".."), 0))
  {
   StackPop(part_stack_top);
  }
  else if(Str8Match(n->string, Str8Lit("."), 0))
  {
   // NOTE(rjf): do nothing
  }
  else
  {
   NodeNode *nn = PushArray(scratch.arena, NodeNode, 1);
   nn->node = n;
   StackPush(part_stack_top, nn);
  }
 }
 String8List result = {0};
 for(NodeNode *nn = part_stack_top; nn != 0; nn = nn->next)
 {
  Str8ListPushFront(arena, &result, nn->node->string);
 }
 ScratchEnd(scratch);
 return result;
}

internal String8
NormalizedPathFromStr8(Arena *arena, String8 source, String8 path)
{
 Temp scratch = ScratchBegin(&arena, 1);
 path = Str8SkipWhitespace(path);
 B32 trailing_slash = path.size > 0 && (path.str[path.size-1] == '/' || path.str[path.size-1] == '\\');
 PathKind kind = PathKindFromStr8(path);
 String8List path_parts = PathPartsFromStr8(scratch.arena, path);
 String8List absolute_path_parts = AbsolutePathPartsFromSourcePartsKind(scratch.arena, source, path_parts, kind);
 String8List absolute_resolved_path_parts = DotResolvedPathPartsFromParts(scratch.arena, absolute_path_parts);
 StringJoin join = {0};
 join.sep = Str8Lit("/");
 if(trailing_slash)
 {
  join.post = Str8Lit("/");
 }
 String8 absolute_resolved_path = Str8ListJoin(scratch.arena, absolute_resolved_path_parts, &join);
 ScratchEnd(scratch);
 return absolute_resolved_path;
}

////////////////////////////////
//~ rjf: Array Functions

internal String8Array
Str8ArrayFromList(Arena *arena, String8List list)
{
 String8Array array = {0};
 array.count = list.node_count;
 array.v = PushArrayNoZero(arena, String8, array.count);
 U64 idx = 0;
 for(String8Node *n = list.first; n != 0; n = n->next, idx += 1)
 {
  array.v[idx] = n->string;
 }
 return array;
}

////////////////////////////////
//~ rjf: Numeric Conversions

internal U64
U64FromStr8(String8 string, U32 radix)
{
 Assert(2 <= radix && radix <= 16);
 local_persist U8 char_to_value[] =
 {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 };
 U64 value = 0;
 for (U64 i = 0; i < string.size; i += 1)
 {
  value *= radix;
  U8 c = string.str[i];
  value += char_to_value[(c - 0x30)&0x1F];
 }
 return value;
}

internal S64
CStyleIntFromStr8(String8 string)
{
 U64 p = 0;
 
 // consume sign
 S64 sign = +1;
 if(p < string.size)
 {
  U8 c = string.str[p];
  if(c == '-')
  {
   sign = -1;
   p += 1;
  }
  else if(c == '+')
  {
   p += 1;
  }
 }
 
 // radix from prefix
 U64 radix = 10;
 if(p < string.size)
 {
  U8 c0 = string.str[p];
  if(c0 == '0')
  {
   p += 1;
   radix = 8;
   if(p < string.size)
   {
    U8 c1 = string.str[p];
    if(c1 == 'x')
    {
     p += 1;
     radix = 16;
    }
    else if(c1 == 'b')
    {
     p += 1;
     radix = 2;
    }
   }
  }
 }
 
 // consume integer "digits"
 String8 digits_substr = Str8Skip(string, p);
 U64 n = U64FromStr8(digits_substr, radix);
 
 // combine result
 S64 result = sign*n;
 return result;
}

internal F64
F64FromStr8(String8 string)
{
 char str[64];
 U64 str_size = string.size;
 if(str_size > sizeof(str) - 1)
 {
  str_size = sizeof(str) - 1;
 }
 MemoryCopy(str, string.str, str_size);
 str[str_size] = 0;
 return atof(str);
}

internal String8
CStyleHexStringFromU64(Arena *arena, U64 x, B32 caps)
{
 local_persist char int_value_to_char[] = "0123456789abcdef";
 U8 buffer[18];
 U8 *opl = buffer + 18;
 U8 *ptr = opl;
 if(x == 0)
 {
  ptr -= 1;
  *ptr = '0';
 }
 else
 {
  for(;;)
  {
   U32 val = x%16;
   x /= 16;
   U8 c = (U8)int_value_to_char[val];
   if(caps)
   {
    c = CharToUpper(c);
   }
   ptr -= 1;
   *ptr = c;
   if (x == 0)
   {
    break;
   }
  }
 }
 ptr -= 1;
 *ptr = 'x';
 ptr -= 1;
 *ptr = '0';
 
 String8 result = {0};
 result.size = (U64)(opl - ptr);
 result.str = PushArrayNoZero(arena, U8, result.size);
 MemoryCopy(result.str, buffer, result.size);
 
 return result;
}

////////////////////////////////
//~ rjf: Text Coordinates

internal TxtPt
TxtPtFromStr8Off(String8 string, U64 off)
{
 TxtPt pt = {1, 1};
 for(U64 idx = 0; idx < string.size && idx < off; idx += 1)
 {
  if(string.str[idx] == '\n')
  {
   pt.line += 1;
   pt.column = 1;
  }
  else
  {
   pt.column += 1;
  }
 }
 return pt;
}
