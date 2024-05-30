/* date = June 6th 2023 5:53 pm */

#ifndef OS_NET_H
#define OS_NET_H

////////////////////////////////
//~ rjf: Receipts

typedef struct OS_InitNetReceipt OS_InitNetReceipt;
struct OS_InitNetReceipt
{
 U64 u64[1];
};

////////////////////////////////
//~ rjf: Protocols

typedef enum OS_NetProtocol
{
 OS_NetProtocol_TCP,
 OS_NetProtocol_UDP,
 OS_NetProtocol_COUNT
}
OS_NetProtocol;

////////////////////////////////
//~ rjf: Address

typedef enum OS_NetAddrFamily
{
 OS_NetAddrFamily_IPV4,
 OS_NetAddrFamily_IPV6,
 OS_NetAddrFamily_COUNT
}
OS_NetAddrFamily;

typedef struct OS_NetAddr OS_NetAddr;
struct OS_NetAddr
{
 OS_NetAddrFamily family;
 union
 {
  // NOTE(rjf): formatted in read order: 127.0.0.1 => [0] = 127, ... [3] = 1
  U8 u8[4];
  U16 u16[2];
  U32 u32[1];
 };
 // NOTE(rjf): local endianness
 U16 port;
};

////////////////////////////////
//~ rjf: Results

typedef struct OS_AcceptResult OS_AcceptResult;
struct OS_AcceptResult
{
 OS_NetAddr addr;
 OS_Handle socket;
};

typedef struct OS_ReceiveResult OS_ReceiveResult;
struct OS_ReceiveResult
{
 OS_NetAddr addr;
 String8 data;
};

////////////////////////////////
//~ rjf: Net Address <=> String

internal OS_NetAddr OS_NetAddrFromString(String8 string);
internal OS_NetAddr OS_NetAddrFromStringAndPort(String8 string, U16 port);
internal String8 OS_StringFromNetAddr(Arena *arena, OS_NetAddr *addr, B32 include_port);

////////////////////////////////
//~ rjf: @os_per_backend Top-Level API

internal OS_InitNetReceipt OS_InitNet(OS_InitReceipt os_init);

////////////////////////////////
//~ rjf: @os_per_backend Sockets

internal OS_Handle OS_SocketAlloc(OS_NetProtocol protocol);
internal void OS_SocketRelease(OS_Handle s);
internal void OS_SocketBind(OS_Handle s, OS_NetAddr addr);
internal OS_AcceptResult OS_SocketAccept(OS_Handle s);
internal void OS_SocketSend(OS_Handle s, String8 data);
internal void OS_SocketSendTo(OS_Handle s, OS_NetAddr addr, String8 data);
internal OS_ReceiveResult OS_SocketReceive(Arena *arena, OS_Handle s, U64 cap);
internal OS_ReceiveResult OS_SocketReceiveFrom(Arena *arena, OS_Handle s, U64 cap);

#endif // OS_NET_H
