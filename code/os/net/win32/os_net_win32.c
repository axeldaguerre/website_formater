////////////////////////////////
//~ rjf: Linker

#pragma comment(lib, "ws2_32.lib")

////////////////////////////////
//~ rjf: Helpers

internal int
OS_W32_AFFromNetAddrFamily(OS_NetAddrFamily fam)
{
 int result = AF_INET;
 switch(fam)
 {
  default:
  case OS_NetAddrFamily_IPV4:{result = AF_INET;}break;
  case OS_NetAddrFamily_IPV6:{result = AF_INET6;}break;
 }
 return result;
}

internal OS_NetAddrFamily
OS_W32_NetAddrFamilyFromAF(int af)
{
 OS_NetAddrFamily result = OS_NetAddrFamily_IPV4;
 switch(af)
 {
  default:
  case AF_INET:{result = OS_NetAddrFamily_IPV4;}break;
  case AF_INET6:{result = OS_NetAddrFamily_IPV6;}break;
 }
 return result;
}

internal struct sockaddr_in
OS_W32_SockAddrInFromNetAddr(OS_NetAddr *net_addr)
{
 struct sockaddr_in result = {0};
 result.sin_family = OS_W32_AFFromNetAddrFamily(net_addr->family);
 result.sin_port = ByteSwapU16(net_addr->port);
 result.sin_addr.S_un.S_un_b.s_b1 = net_addr->u8[0];
 result.sin_addr.S_un.S_un_b.s_b2 = net_addr->u8[1];
 result.sin_addr.S_un.S_un_b.s_b3 = net_addr->u8[2];
 result.sin_addr.S_un.S_un_b.s_b4 = net_addr->u8[3];
 return result;
}

internal OS_NetAddr
OS_W32_NetAddrFromSockAddrIn(struct sockaddr_in *sock_addr)
{
 OS_NetAddr result = {0};
 result.family = OS_W32_NetAddrFamilyFromAF(sock_addr->sin_port);
 result.port = ByteSwapU16(sock_addr->sin_port);
 result.u8[0] = sock_addr->sin_addr.S_un.S_un_b.s_b1;
 result.u8[1] = sock_addr->sin_addr.S_un.S_un_b.s_b2;
 result.u8[2] = sock_addr->sin_addr.S_un.S_un_b.s_b3;
 result.u8[3] = sock_addr->sin_addr.S_un.S_un_b.s_b4;
 return result;
}

////////////////////////////////
//~ rjf: @os_per_backend Top-Level API

internal OS_InitNetReceipt
OS_InitNet(OS_InitReceipt os_init)
{
 WSADATA wsa_data = {0};
 int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
 OS_InitNetReceipt receipt = {0};
 return receipt;
}

////////////////////////////////
//~ rjf: @os_per_backend Sockets

internal OS_Handle
OS_SocketAlloc(OS_NetProtocol protocol)
{
 int af = 0;
 int type = 0;
 int prot = 0;
 switch(protocol)
 {
  default:
  case OS_NetProtocol_TCP:
  {
   af = AF_INET;
   type = SOCK_STREAM;
   prot = IPPROTO_TCP;
  }break;
  case OS_NetProtocol_UDP:
  {
   af = AF_INET;
   type = SOCK_DGRAM;
   prot = IPPROTO_UDP;
  }break;
 }
 SOCKET s = socket(af, type, prot);
 OS_Handle s_handle = {(U64)s};
 return s_handle;
}

internal void
OS_SocketRelease(OS_Handle s)
{
 SOCKET sock = (SOCKET)s.u64[0];
 closesocket(sock);
}

internal void
OS_SocketBind(OS_Handle s, OS_NetAddr addr)
{
 SOCKET sock = (SOCKET)s.u64[0];
 struct sockaddr_in addrin = OS_W32_SockAddrInFromNetAddr(&addr);
 int error = bind(sock, (struct sockaddr *)&addrin, sizeof(addrin));
}

internal OS_AcceptResult
OS_SocketAccept(OS_Handle s)
{
 SOCKET sock = (SOCKET)s.u64[0];
 int error = 0;
 error = listen(sock, SOMAXCONN);
 struct sockaddr_in connected_addr = {0};
 int connected_addr_size = (int)sizeof(connected_addr);
 SOCKET connected_sock = accept(sock, (struct sockaddr *)&connected_addr, &connected_addr_size);
 OS_AcceptResult result = zero_struct;
 result.addr = OS_W32_NetAddrFromSockAddrIn(&connected_addr);
 result.socket.u64[0] = (U64)connected_sock;
 return result;
}

internal void
OS_SocketSend(OS_Handle s, String8 data)
{
 SOCKET sock = (SOCKET)s.u64[0];
 int error = send(sock, (char *)data.str, data.size, 0);
}

internal void
OS_SocketSendTo(OS_Handle s, OS_NetAddr addr, String8 data)
{
 SOCKET sock = (SOCKET)s.u64[0];
 struct sockaddr_in dst_addr = OS_W32_SockAddrInFromNetAddr(&addr);
 int error = sendto(sock, (char *)data.str, data.size, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr));
}

internal OS_ReceiveResult
OS_SocketReceive(Arena *arena, OS_Handle s, U64 cap)
{
 SOCKET sock = (SOCKET)s.u64[0];
 U8 *buffer = PushArrayNoZero(arena, U8, cap);
 int error = recv(sock, (char *)buffer, cap, 0);
 U64 bytes_received = 0;
 if(error > 0)
 {
  bytes_received = (U64)error;
 }
 OS_ReceiveResult result = zero_struct;
 result.data = Str8(buffer, bytes_received);
 return result;
}

internal OS_ReceiveResult
OS_SocketReceiveFrom(Arena *arena, OS_Handle s, U64 cap)
{
 SOCKET sock = (SOCKET)s.u64[0];
 U8 *buffer = PushArrayNoZero(arena, U8, cap);
 struct sockaddr_in recv_addr = {0};
 int recv_addr_size = sizeof(recv_addr);
 int error = recvfrom(sock, (char *)buffer, cap, 0, (struct sockaddr *)&recv_addr, &recv_addr_size);
 U64 bytes_received = 0;
 if(error > 0)
 {
  bytes_received = (U64)error;
 }
 OS_ReceiveResult result = zero_struct;
 result.addr = OS_W32_NetAddrFromSockAddrIn(&recv_addr);
 result.data = Str8(buffer, bytes_received);
 return result;
}
