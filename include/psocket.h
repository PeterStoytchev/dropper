#pragma once

#include "types.h"

// This is temporary
#define OS_WINDOWS

#ifdef OS_WINDOWS

    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>

    typedef SOCKET psocket_raw;

#else

    typedef s32 psocket_raw;

#endif

#define NETWORK_PORT 13337
#define NETWORK_PORT_BROADCAST 13338

enum {
    PROTO_NONE,
    PROTO_TCP,
    PROTO_UDP
} psocket_proto;

enum {
    STYPE_NONE,
    STYPE_CLIENT,
    STYPE_SERVER
} psocket_type;

struct psocket
{
    enum psocket_type type;
    enum psocket_proto proto;
    psocket_raw handle;
};


struct psocket CreateSocket(enum psocket_type type, enum psocket_proto proto, u32 ip, u16 port);
void CloseSocket(struct psocket socket);

struct psocket AcceptSocket(struct psocket server_socket);

s8 ReadFromSocket(struct psocket socket, s32 size, void* dst_memory);
void WriteToSocket(struct psocket socket, s32 size, void* src_memory);

// Gets the IP of the device on the other side of the socket
u32 GetRemoteAddress(struct psocket s);

// For UDP, there is no way to get the remote adress whenever,
// but we can get it when reading. This function does that!
u32 ReadFromSocket_GetIncoming(struct psocket s, s32 size, void* dst_memory);

//@Note: Only supports IPv4
char* IPtoString(u32 ip);