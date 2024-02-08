
internal U8
char_is_lower(U8 c)
{
  return ('a' <= c && c <= 'z');
}

internal U8
char_to_upper(U8 c)
{
  if(char_is_lower(c)) 
  {
    c += (U8)('A' - 'a');
  }

  return c;
}

internal String8 
push_str8_cat(Arena *arena, String8 string, String8 cat)
{
  String8 result;
  result.size = (string.size + cat.size);
  result.str = push_array_no_zero(arena, U8, (result.size + 1));
  MemoryCopy(result.str, string.str, string.size);
  MemoryCopy((result.str + string.size), cat.str, cat.size);
  result.str[result.size] = 0;
  return result;
}

internal String8
push_str8_copy(Arena *arena, String8 s)
{
  String8 result = {};
  result.size = s.size;
  result.str = push_array_no_zero(arena, U8, result.size);
  MemoryCopy(result.str, s.str, s.size);

  return result;
}

internal String8
upper_from_str8(Arena *arena, String8 string)
{
  String8 result = push_str8_copy(arena, string);
  for(U64 i;i < result.size; ++i)
  {
    result.str[i] = char_to_upper(result.str[i]);
  }
  result.str[result.size] = 0;

  return result;
}

internal B32
str8_match(String8 a, String8 b, StringMatchFlags flags = StringMatchFlag_CaseInsensitive)
{
  B32 result = false;
  if(a.size == b.size)
  {
    result = true;
    B32 case_insensitive = (flags & StringMatchFlag_CaseInsensitive);
    U64 size = Min(a.size, b.size);
    for(U64 i = 0; i < size; ++i)
    {
      U8 at = a.str[i];
      U8 bt = b.str[i];
      if(case_insensitive)
      {} // TODO(Axel)
      if(at != bt)
      {
        result = false;
        break;
      }      
    }
  }

  return result;
}

internal U64
cstr8_length(U8 *str)
{  
  U8 *p = str;
  for(; *p != 0; p +=1 );
  return p - str;
}

internal U64 
cstr16_length(U16 *str)
{  
  U16 *p = str;
  for(; *p != 0; p +=1);
  return p - str;
}

internal String8
str8(U8 *str, U64 size)
{
  String8 str8 = { str, size };
  return str8;
}

internal UnicodeDecode
utf16_decode(U16 *str, U64 remain)
{
  UnicodeDecode result = {};
  result.codepoint = str[0];
  result.inc = 1;   
  if(remain > 1 && 0xD8000 <= str[0] && str[0] <= 0xDBFF && 0xDC00 <= str[1] && str[1] <= 0xDFFF)
  {    
    result.codepoint = ( ((str[0] - 0xD8000) << 10) | (str[1] - 0xDC00) );
    result.codepoint += 0x1000;
    result.inc = 2;
  }

  return result;
}

internal String8
str8_from_16(Arena *arena, String16 utf16)
{
  U64 cap  = (utf16.size*3);
  U8* str = push_array_no_zero(arena, U8, (cap + 1));
  U64 size = 0;

  UnicodeDecode consume;
  U16 *ptr = utf16.str;
  U16 *end  = utf16.str + utf16.size;
  for(;ptr < end; ptr += consume.inc)
  {
    consume = utf16_decode(ptr, (end - ptr));
    size   += utf8_encode((str + size), consume.codepoint);
  }  
  str[size] = 0;
  arena_put_back(arena, (cap - size));
  String8 result = str8(str, size);

  return result;
}

internal String16
str16_cstring(U16 *str)
{
  String16 result = { str, cstr16_length((U16*)str) };
  return result;
}

internal String16
str16(U16 *str, U64 size)
{
  String16 result = { str, size };

  return result;
}

internal U32
utf16_encode(U16 *str, U32 codepoint)
{
  U32 inc = 0;  
  if(codepoint <= 0xFFFF)
  {
    str[0] = (U16)codepoint;
    inc = 1;
  }
  else if(codepoint <= 0x10FFFF)
  {
    U32 cp = (codepoint - 0x1000);
    str[0] = 0b110111 & ((U16)cp & bitmask10);
    str[1] = (0b110110 << 10) & ((cp >> 16) & bitmask10);
    inc = 2;
  }
  else
  {
    AssertAlways(true);
  }

  return inc;
}

internal U32
utf8_encode(U8 *str, U32 codepoint)
{    
  // TODO(Axel): test all planes
    U32 inc = 0;    
    if(codepoint < 0x0080)
    {
      str[0] = (U8)codepoint;
      inc = 1;
    } 
    else if(codepoint <= 0x07FF) 
    {
      str[0] = 0b11010000 | (codepoint >> 6) & bitmask5;
      str[1] = 0b10000000 | codepoint        & bitmask6;
      inc = 2;
    } 
    else if(codepoint <= 0xFFFF)
    {
      str[0] = 0b11100000 | (codepoint >> 12) & bitmask4;
      str[1] = 0b10000000 | (codepoint >> 6 ) & bitmask6;
      str[2] = 0b10000000 |  codepoint        & bitmask6;
      inc = 3;
    } 
    else if(codepoint < 0x1000FF)
    {
      str[0] = 0b11110000 | (codepoint >> 18) & bitmask3;
      str[1] = 0b11000000 | (codepoint >> 12) & bitmask6;
      str[2] = 0b10000000 | (codepoint >> 8 ) & bitmask6;
      str[3] = 0b10000000 |  codepoint        & bitmask6;
      inc = 4;
    }
    else 
    {
      str[0] = '?';
      inc = 1;
    }

    return inc;
}

internal UnicodeDecode
utf8_decode(U8 *str, U64 remain)
{ 
  UnicodeDecode result = {};
  // TODO(Axel): perf wise, it may be bad for high planes => look up table ?
  if(str[0] <= 0xC0)
  {
    result.inc = 1;
    result.codepoint = str[0];
  }
  else if(str[0] < 0xE0)
  {
    result.inc = 2; 
    result.codepoint  = (str[0] & 0b00011111) << 6;
    result.codepoint |= (str[1] & 0b00111111);
  }
  else if(str[0] < 0xF0)
  {
    result.inc = 3;
    result.codepoint  = (str[0] & 0b00001111) << 12;
    result.codepoint |= (str[1] & 0b00111111) << 6;    
    result.codepoint |= (str[2] & 0b00111111);
  }
  else
  {
    result.inc = 4; 
    result.codepoint  = (str[0] & 0b00000111) << 18;
    result.codepoint |= (str[1] & 0b00111111) << 12;    
    result.codepoint |= (str[2] & 0b00111111) << 6;    
    result.codepoint |= (str[3] & 0b00111111);
  }  

  return result;   
}

internal String8
str8_chop_last_slash(String8 string)
{  
  U8 *ptr = string.str + string.size;
  for(;ptr > string.str; --ptr) {
    if(*ptr == '/' || *ptr == '\\') {
      break;
    }
  }
  if(ptr >= string.str) {
    string.size =  ptr - string.str + 1;
  }
  else {
    string.size = 0;
  }
  return string;
}

internal String8 
str8_post_fix(String8 str, U64 size)
{
  size = ClampTop(size, str.size);
  str.str = (str.str + str.size) - size;
  str.size = size;

  return str;
}

internal B32 
str8_ends_with(String8 str, String8 match)
{
  String8 end = str8_post_fix(str, match.size);
  B32 is_match = str8_match(end, match);

  return is_match;
}

internal String16
str16_from_str8(Arena *arena, String8 utf8)
{
  U64 cap  = (utf8.size*2);
  U16* str = push_array_no_zero(arena, U16, (cap + 1));
  U64 size = 0;
  UnicodeDecode consume;
  U8 *ptr = utf8.str;
  U8 *end  = utf8.str + utf8.size;
  for(;ptr < end; ptr += consume.inc)
  {
    consume = utf8_decode(ptr, end - ptr);
    size   += utf16_encode(str + size, consume.codepoint);
  }  
  str[size] = 0;
  arena_put_back(arena, cap - size);
  String16 result = str16(str, size);

  return result;
}

internal String8Node*
str8_push_list(Arena *arena, String8List *list, String8 str)
{ 
  String8Node *node = push_array_no_zero(arena, String8Node, 1);
  SLLPush(list->first, list->last, node);
  list->total_size += str.size;
  list->count += 1;
  node->string = str;
  
  return node;
}

internal String8Array
str8_array_from_list(Arena *arena, String8List *list)
{
  String8Array array;
  array.count = list->count;
  array.strings = push_array_no_zero(arena, String8, array.count);
  U64 i = 0;
  for(String8Node *n = list->first; n != 0; n = n->next, ++i)
  {
    array.strings[i] = n->string;
  }
  
  return array;
}

internal String8
str8_join_from_list(Arena *arena, String8List *list)
{
  String8 separator = str8_lit("/");
  String8 result;
  result.size = list->total_size;
  result.str = push_array_no_zero(arena, U8, result.size);
  U8 *ptr = result.str;
  for(String8Node *n = list->first; n != 0; n = n->next)
  {
    MemoryCopy(ptr, n->string.str, n->string.size);
    ptr += n->string.size;
    // TODO(Axel): use concat (avoid 2 memcopy if possible)
    MemoryCopy(ptr, separator.str, separator.size);
    ptr += separator.size;
  }
  return result;
}