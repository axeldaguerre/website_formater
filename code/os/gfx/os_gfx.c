////////////////////////////////
//~ rjf: Generated Code

#include "generated/os_gfx.meta.c"

////////////////////////////////
//~ rjf: Basic Helpers

internal String8
OS_StringFromKey(OS_Key key)
{
 return os_g_key_string_table[key];
}

internal String8
OS_StringFromModifiersKey(Arena *arena, OS_Modifiers modifiers, OS_Key key)
{
 Temp scratch = ScratchBegin(&arena, 1);
 String8 key_string = OS_StringFromKey(key);
 String8List modifiers_strings = {0};
 if(modifiers & OS_Modifier_Ctrl)
 {
  Str8ListPush(scratch.arena, &modifiers_strings, Str8Lit("Ctrl"));
 }
 if(modifiers & OS_Modifier_Shift)
 {
  Str8ListPush(scratch.arena, &modifiers_strings, Str8Lit("Shift"));
 }
 if(modifiers & OS_Modifier_Alt)
 {
  Str8ListPush(scratch.arena, &modifiers_strings, Str8Lit("Alt"));
 }
 StringJoin join = {0};
 join.sep = Str8Lit(" + ");
 String8 modifiers_string = Str8ListJoin(scratch.arena, modifiers_strings, &join);
 String8 final_string = PushStr8F(arena, "%S%s%S", modifiers_string, modifiers_string.size != 0 ? " + " : "", key_string);
 ScratchEnd(scratch);
 return final_string;
}

////////////////////////////////
//~ rjf: Event Helpers

internal U64
OS_CharacterFromModifiersAndKey(OS_Modifiers modifiers, OS_Key key)
{
 U64 character = 0;
 if(key >= OS_Key_A && key <= OS_Key_Z)
 {
  character = (modifiers & OS_Modifier_Shift) ? 'A' + (key-OS_Key_A) : 'a' + (key-OS_Key_A);
 }
 else
 {
  local_persist struct
  {
   OS_Key key;
   U64 char_no_shift;
   U64 char_shift;
  }
  map[] =
  {
   { OS_Key_Space, ' ', ' ' },
   { OS_Key_1, '1', '!' },
   { OS_Key_2, '2', '@' },
   { OS_Key_3, '3', '#' },
   { OS_Key_4, '4', '$' },
   { OS_Key_5, '5', '%' },
   { OS_Key_6, '6', '^' },
   { OS_Key_7, '7', '&' },
   { OS_Key_8, '8', '*' },
   { OS_Key_9, '9', '(' },
   { OS_Key_0, '0', ')' },
   { OS_Key_Minus, '-', '_' },
   { OS_Key_Equal, '=', '+' },
   { OS_Key_GraveAccent, '`', '~' },
   { OS_Key_LeftBracket, '[', '{' },
   { OS_Key_RightBracket, ']', '}' },
   { OS_Key_Quote, '\'', '"' },
   { OS_Key_Comma, ',', '<' },
   { OS_Key_Period, '.', '>' },
   { OS_Key_ForwardSlash, '/', '?' },
   { OS_Key_Enter, '\n', '\n' },
   { OS_Key_Semicolon, ';', ':' },
  };
  for(int i = 0; i < ArrayCount(map); ++i)
  {
   if(key == map[i].key)
   {
    if(modifiers & OS_Modifier_Shift)
    {
     character = map[i].char_shift;
    }
    else
    {
     character = map[i].char_no_shift;
    }
    break;
   }
  }
 }
 return character;
}

internal OS_CursorKind
OS_CursorKindFromResizeSides(Side x, Side y)
{
 OS_CursorKind kind = OS_CursorKind_Pointer;
 struct
 {
  Side x;
  Side y;
  OS_CursorKind kind;
 }
 map[] =
 {
  { Side_Min, Side_Invalid, OS_CursorKind_WestEast},
  { Side_Max, Side_Invalid, OS_CursorKind_WestEast},
  { Side_Invalid, Side_Min, OS_CursorKind_NorthSouth},
  { Side_Invalid, Side_Max, OS_CursorKind_NorthSouth},
  { Side_Min, Side_Min, OS_CursorKind_NorthWestSouthEast },
  { Side_Min, Side_Max, OS_CursorKind_NorthEastSouthWest},
  { Side_Max, Side_Min, OS_CursorKind_NorthEastSouthWest},
  { Side_Max, Side_Max, OS_CursorKind_NorthWestSouthEast },
 };
 
 for(int i = 0; i < ArrayCount(map); i += 1)
 {
  if(x == map[i].x && y == map[i].y)
  {
   kind = map[i].kind;
   break;
  }
 }
 return kind;
}

internal String8
OS_StringFromEvent(Arena *arena, OS_Event *event)
{
 String8 string = {0};
 if(event)
 {
  String8 main_control_string = {0};
  U8 character = 0;
  
  if(event->key != OS_Key_Null)
  {
   main_control_string = OS_StringFromKey(event->key);
  }
  else if(event->character != 0)
  {
   character = (U8)event->character;
   main_control_string = Str8(&character, 1);
  }
  if(main_control_string.size > 0)
  {
   String8 mod_string =
    PushStr8F(arena, "%s%s%s",
              (event->modifiers & OS_Modifier_Ctrl) ? "Ctrl + " : "",
              (event->modifiers & OS_Modifier_Shift) ? "Shift + " : "",
              (event->modifiers & OS_Modifier_Alt) ? "Alt + " : "");
   string = PushStr8F(arena, "%.*s%.*s",
                      Str8VArg(mod_string),
                      Str8VArg(main_control_string));
  }
 }
 return string;
}

internal B32
OS_KeyPress(OS_EventList *events, OS_Handle window, OS_Key key, OS_Modifiers mods)
{
 B32 result = 0;
 for(OS_Event *e = events->first; e != 0; e = e->next)
 {
  if(e->kind == OS_EventKind_Press && OS_HandleMatch(window, e->window) &&
     e->key == key &&
     (e->modifiers & mods != 0 || (e->modifiers == 0 && mods == 0)))
  {
   OS_EatEvent(events, e);
   result = 1;
   break;
  }
 }
 return result;
}

internal B32
OS_KeyRelease(OS_EventList *events, OS_Handle window, OS_Key key, OS_Modifiers mods)
{
 B32 result = 0;
 for(OS_Event *e = events->first; e != 0; e = e->next)
 {
  if(e->kind == OS_EventKind_Release && OS_HandleMatch(window, e->window) &&
     e->key == key &&
     (e->modifiers & mods != 0 || (e->modifiers == 0 && mods == 0)))
  {
   OS_EatEvent(events, e);
   result = 1;
   break;
  }
 }
 return result;
}

internal B32
OS_TextCodepoint(OS_EventList *events, OS_Handle window, U32 codepoint)
{
 B32 result = 0;
 for(OS_Event *e = events->first, *next = 0; e != 0; e = next)
 {
  next = e->next;
  if(e->kind == OS_EventKind_Text && OS_HandleMatch(window, e->window) && e->character == codepoint)
  {
   OS_EatEvent(events, e);
   result = 1;
   break;
  }
 }
 return result;
}
