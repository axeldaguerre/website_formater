
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
push_str8_copy(Arena *arena, String8 s)
{
  String8 result = {0};
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

internal U64
str8_find(String8 string, String8 match, StringMatchFlags flags)
{
  U8 *ptr = string.str;
  U8 *end = string.str + string.size;
  
  for(;ptr <= end; ptr += 1){
    B32 is_match = 1;
    for(U64 i = 0; i < match.size; ++i){
      if(*(ptr + i) != match.str[i]){
        is_match = 0;
        break;
      }
    }
    if(is_match){
        break;
    }
  }
  U64 result = string.size;
  if(ptr != end){
    result = (U64)(ptr - string.str);
  }
  return result;
}

internal B32
str8_match(String8 a, String8 b, StringMatchFlags flags)
{
  flags |= !flags ? StringMatchFlag_CaseInsensitive : flags;
  B32 result = 0;
  if(a.size == b.size)
  {
    result = 1;
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
        result = 0;
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
  UnicodeDecode result = {0};
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
  return(str8(str, size));
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
    AssertAlways(1);
  }

  return inc;
}

internal U32
utf8_encode(U8 *str, U32 codepoint)
{
    U32 inc = 0;
    if(codepoint < 0x0080)
    {
      str[0] = (U8)codepoint;
      inc = 1;
    } 
    else if(codepoint <= 0x07FF)
    {
      str[0] = (U8)((0b11000000) | ((codepoint >> 6) & bitmask5));
      str[1] = (U8)((0b10000000) | (codepoint        & bitmask6));
      inc = 2;
    } 
    else if(codepoint <= 0xFFFF)
    {
      str[0] = (U8)((0b11100000) | ((codepoint >> 12) & bitmask4));
      str[1] = (U8)((0b10000000) | ((codepoint >> 6 ) & bitmask6));
      str[2] = (U8)((0b10000000) |  (codepoint        & bitmask6));
      inc = 3;
    } 
    else if(codepoint < 0x1000FF)
    {
      str[0] = (U8)((0b11110000) | ((codepoint >> 18) & bitmask3));
      str[1] = (U8)((0b11000000) | ((codepoint >> 12) & bitmask6));
      str[2] = (U8)((0b10000000) | ((codepoint >> 8 ) & bitmask6));
      str[3] = (U8)((0b10000000) |  (codepoint        & bitmask6));
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
  UnicodeDecode result = {0};
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
push_str8_cat(Arena *arena, String8 s1, String8 s2)
{
  String8 string;
  string.size = s1.size + s2.size;
  string.str = push_array_no_zero(arena, U8, string.size + 1);
  MemoryCopy(string.str, s1.str, s1.size);
  MemoryCopy(string.str + s1.size, s2.str, s2.size);
  string.str[string.size] = 0;
  return(string);
}

internal String8
str8_from_last_slash(String8 string)
{    
  if(string.size > 0){
    U8 *ptr = string.str + string.size - 1;
    for(;ptr > string.str; --ptr){
      if(*ptr == '\\' || *ptr == '/'){
        ++ptr;
        break;
      }
    }
    
    string.size = (U64)(string.str + string.size - ptr);
    string.str = ptr;
  }
  return string; 
}

internal String8
str8_chop_last_slash(String8 string)
{  
  U8 *ptr = string.str + string.size -1;
  for(;ptr > string.str; --ptr) {
    if(*ptr == '/' || *ptr == '\\') {
      ++ptr;
      break;
    }
  }
  if(ptr >= string.str) {
    string.size =  (U64)(ptr - string.str);
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
  B32 is_match = str8_match(end, match, StringMatchFlag_CaseInsensitive);

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
str8_list_join(Arena *arena, String8List *list, String8 separator)
{  
  String8 result;
  result.size = list->total_size;
  result.str = push_array_no_zero(arena, U8, result.size + 1);
  U8 *ptr = result.str;
  for(String8Node *node = list->first;
      node != 0;
      node = node->next)
  {
    MemoryCopy(ptr, node->string.str, node->string.size);
    ptr += node->string.size;
    // TODO(Axel): use concat (avoid 2 memcopy if possible)
    if(node->next != 0){
      MemoryCopy(ptr, separator.str, separator.size);
      ptr += separator.size;
    }
  }
  *ptr = 0;
  return result;
}

internal String8
str8_range(U8 *first, U8 *one_past_last)
{
  String8 result = {first, (U64)(one_past_last - first)};
  return result;
} 

/*
  Note: one char singularity only, you can't split with more than 
        one character singularity
*/
internal void
str8_split_push_list(Arena *arena, String8List *list, U8 *split_chars, U64 split_chars_count, 
                    String8 string, StringSplitFlags flags)
{
  U8 *ptr = string.str;  
  U8 *end = string.str + string.size;
  
  B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
  
  for(;ptr <= end;){
    U8 *start = ptr;
    for(;ptr <= end;ptr += 1){
      B32 is_split = 0;
      U8 c = *ptr;
      for(U64 i = 0; i <= split_chars_count; ++i){
        // TODO, if the full string of split chars, it may be stuck in the loop        
        if(c == split_chars[i]){
          is_split = 1;
          break;
        }
      }
      if(is_split == 1){
          break;
      }
    }
    String8 str = str8_range(start, ptr);    
    
    if(keep_empties || str.size > 0){
      str8_push_list(arena, list, str);
    }    
    ++ptr;
  } 
}

/*
  NOTE: List.count == 1 when no split has occured
*/
internal String8List 
str8_split(Arena *arena, String8 string, String8 split, StringSplitFlags flags)
{
  String8List list = {0}; 
  str8_split_push_list(arena, &list, split.str, split.size, string, flags);
  return list;
}

internal String8
str8_cut_from_last_dot(String8 string)
{
  U8 *ptr = string.str + string.size;
  String8 result = string;
  result.str = string.str;
  result.size = string.size;
  for(;ptr > string.str; --ptr){
    if(*ptr == '.'){
      result.str = ptr + 1;
      result.size = (U64)(ptr - string.str);
     break; 
    }
  }
  return result;
}


