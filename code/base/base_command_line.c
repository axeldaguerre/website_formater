
internal U64
CmdLineHashFromString(String8 string)
{
 U64 result = 5381;
 for(U64 i = 0; i < string.size; i += 1)
 {
  result = ((result << 5) + result) + string.str[i];
 }
 return result;
}

internal CmdLine
CmdLineFromStringList(Arena *arena, String8List strings)
{
 Temp scratch = ScratchBegin(&arena, 1);
 
 //- rjf: set up
 CmdLine cmdln = {0};
 cmdln.slots_count = 64;
 cmdln.slots = PushArray(arena, CmdLineOptSlot, cmdln.slots_count);
 
 //- rjf: separate strings by whitespace & collapse quotes - emulate shell
 String8List separated_strings = {0};
 for(String8Node *n = strings.first; n != 0; n = n->next)
 {
  String8List strings_from_this_n = {0};
  U64 start_idx = 0;
  B32 quoted = 0;
  B32 seeking_non_ws = 0;
  for(U64 idx = 0; idx <= n->string.size; idx += 1)
  {
   if(seeking_non_ws && idx < n->string.size && !CharIsSpace(n->string.str[idx]))
   {
    seeking_non_ws = 0;
    start_idx = idx;
   }
   if(!seeking_non_ws && (idx == n->string.size || n->string.str[idx] == ' ' || n->string.str[idx] == '"'))
   {
    String8 string = Substr8(n->string, R1U64(start_idx, idx));
    Str8ListPush(scratch.arena, &strings_from_this_n, string);
    start_idx = idx+1;
    if(n->string.str[idx] == ' ')
    {
     seeking_non_ws = 1;
    }
   }
   if(idx < n->string.size && n->string.str[idx] == '"')
   {
    quoted ^= 1;
   }
  }
  Str8ListConcatInPlace(&separated_strings, &strings_from_this_n);
 }
 
 //- rjf: parse list of strings
 CmdLineOptNode *active_opt_node = 0;
 for(String8Node *n = separated_strings.first; n != 0; n = n->next)
 {
  String8 piece = Str8SkipChopWhitespace(n->string);
  B32 double_dash   = Str8Match(Prefix8(piece, 2), Str8Lit("--"), 0);
  B32 single_dash   = Str8Match(Prefix8(piece, 1), Str8Lit("-"), 0);
  B32 value_for_opt = (active_opt_node != 0);
  
  //- rjf: dispatch to rule
  if(value_for_opt == 0 && (double_dash || single_dash))
  {
   goto new_option;
  }
  if(value_for_opt)
  {
   goto value_strings;
  }
  
  //- rjf: parse this string as a new option
  new_option:;
  {
   U64 dash_prefix_size = !!single_dash + !!double_dash;
   String8 opt_part = Str8Skip(piece, dash_prefix_size);
   U64 colon_pos = FindSubstr8(opt_part, Str8Lit(":"), 0, 0);
   U64 equal_pos = FindSubstr8(opt_part, Str8Lit("="), 0, 0);
   U64 value_specifier_pos = Min(colon_pos, equal_pos);
   String8 opt_name = Prefix8(opt_part, value_specifier_pos);
   String8 first_part_of_opt_value = Str8Skip(opt_part, value_specifier_pos+1);
   U64 hash = CmdLineHashFromString(opt_name);
   U64 slot_idx = hash%cmdln.slots_count;
   CmdLineOptSlot *slot = &cmdln.slots[slot_idx];
   CmdLineOptNode *node = PushArray(arena, CmdLineOptNode, 1);
   QueuePush(slot->first, slot->last, node);
   node->name = opt_name;
   if(first_part_of_opt_value.size != 0)
   {
    Str8ListPush(arena, &node->values, first_part_of_opt_value);
   }
   if(value_specifier_pos < opt_part.size &&
      (first_part_of_opt_value.size == 0 ||
       Str8Match(Suffix8(first_part_of_opt_value, 1), Str8Lit(","), 0)))
   {
    active_opt_node = node;
   }
  }
  goto end_node;
  
  //- rjf: parse this string as containing values
  value_strings:;
  {
   String8 splits[] = {Str8Lit(",")};
   String8List value_parts = StrSplit8(arena, piece, ArrayCount(splits), splits);
   Str8ListConcatInPlace(&active_opt_node->values, &value_parts);
   if(!Str8Match(Suffix8(piece, 1), Str8Lit(","), 0))
   {
    active_opt_node = 0;
   }
  }
  goto end_node;
  
  //- rjf: interpret this string as an input string independent from options
  {
   Str8ListPush(arena, &cmdln.inputs, piece);
  }
  goto end_node;
  
  end_node:;
 }
 
 //- rjf: produce flattened value strings
 {
  for(U64 slot_idx = 0; slot_idx < cmdln.slots_count; slot_idx += 1)
  {
   for(CmdLineOptNode *n = cmdln.slots[slot_idx].first; n != 0; n = n->next)
   {
    StringJoin join = {Str8Lit(""), Str8Lit(","), Str8Lit("")};
    n->value = Str8ListJoin(arena, n->values, &join);
   }
  }
 }
 
 ScratchEnd(scratch);
 return cmdln;
}

internal String8List
CmdLineOptStrings(CmdLine *cmdln, String8 name)
{
 String8List result = {0};
 {
  U64 hash = CmdLineHashFromString(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(Str8Match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = node->values;
  }
 }
 return result;
}

internal String8
CmdLineOptString(CmdLine *cmdln, String8 name)
{
 String8 result = {0};
 {
  U64 hash = CmdLineHashFromString(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(Str8Match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = node->value;
  }
 }
 return result;
}

internal B32
CmdLineOptB32(CmdLine *cmdln, String8 name)
{
 B32 result = 0;
 {
  U64 hash = CmdLineHashFromString(name);
  U64 slot_idx = hash%cmdln->slots_count;
  CmdLineOptSlot *slot = &cmdln->slots[slot_idx];
  CmdLineOptNode *node = 0;
  for(CmdLineOptNode *n = slot->first; n != 0; n = n->next)
  {
   if(Str8Match(n->name, name, 0))
   {
    node = n;
    break;
   }
  }
  if(node != 0)
  {
   result = (node->value.size == 0 ||
             Str8Match(node->value, Str8Lit("true"), MatchFlag_CaseInsensitive) ||
             Str8Match(node->value, Str8Lit("1"), MatchFlag_CaseInsensitive));
  }
 }
 return result;
}

internal F64
CmdLineOptF64(CmdLine *cmdln, String8 name)
{
 F64 result = 0;
 {
  String8 string = CmdLineOptString(cmdln, name);
  result = F64FromStr8(string);
 }
 return result;
}

internal S64
CmdLineOptS64(CmdLine *cmdln, String8 name)
{
 S64 result = 0;
 {
  String8 string = CmdLineOptString(cmdln, name);
  result = CStyleIntFromStr8(string);
 }
 return result;
}
