////////////////////////////////
//~ rjf: Net Address <=> String

internal OS_NetAddr
OS_NetAddrFromString(String8 string)
{
 U64 colon_pos = FindSubstr8(string, Str8Lit(":"), 0, 0);
 U16 port = 0;
 if(colon_pos < string.size)
 {
  String8 port_portion = Str8Skip(string, colon_pos+1);
  S64 port_value = CStyleIntFromStr8(port_portion);
  port = (U16)port_value;
 }
 OS_NetAddr result = OS_NetAddrFromStringAndPort(string, port);
 return result;
}

internal OS_NetAddr
OS_NetAddrFromStringAndPort(String8 string, U16 port)
{
 OS_NetAddr result = {0};
 result.family = OS_NetAddrFamily_IPV4;
 result.port = port;
 {
  U64 find_pos = 0;
  for(U64 dst_byte_idx = 0; dst_byte_idx < 4; dst_byte_idx += 1)
  {
   U64 next_dot_pos = FindSubstr8(string, Str8Lit("."), find_pos+1, 0);
   U64 next_colon_pos = FindSubstr8(string, Str8Lit(":"), find_pos+1, 0);
   U64 next_boundary_pos = Min(next_dot_pos, next_colon_pos);
   if(next_boundary_pos >= string.size)
   {
    break;
   }
   String8 next_byte_string = Substr8(string, R1U64(find_pos, next_boundary_pos));
   find_pos = next_boundary_pos+1;
   S64 next_byte_value = CStyleIntFromStr8(next_byte_string);
   result.u8[dst_byte_idx] = (U8)next_byte_value;
  }
 }
 return result;
}

internal String8
OS_StringFromNetAddr(Arena *arena, OS_NetAddr *addr, B32 include_port)
{
 String8 result = {0};
 switch(addr->family)
 {
  default:{NotImplemented;}break;
  case OS_NetAddrFamily_IPV4:
  {
   switch(include_port)
   {
    default:
    {
     result = PushStr8F(arena, "%i.%i.%i.%i:%i", (int)addr->u8[0], (int)addr->u8[1], (int)addr->u8[2], (int)addr->u8[3], (int)addr->port);
    }break;
    case 0:
    {
     result = PushStr8F(arena, "%i.%i.%i.%i", (int)addr->u8[0], (int)addr->u8[1], (int)addr->u8[2], (int)addr->u8[3]);
    }break;
   }
  }break;
 }
 return result;
}
