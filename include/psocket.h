#pragma once

#include "types.h"

// This is temporary
#define OS_WINDOWS

#ifdef OS_WINDOWS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define NETWORK_PORT "13337"
#define NETWORK_PORT_BROADCAST "13338"

typedef SOCKET psocket_t;

#else

typedef s32 psocket_t;

#define NETWORK_PORT 13337
#define NETWORK_PORT_BROADCAST 13338

#endif


psocket_t OpenSocket();
psocket_t OpenSocketAtDestination(const char* dst);
psocket_t OpenSocketUDPServer();
psocket_t OpenSocketBroadcast();

void CloseSocket(psocket_t socket);

void ListenSocket(psocket_t socket);
psocket_t AcceptSocket(psocket_t server_socket);

void WriteToSocket(psocket_t socket, s32 size, void* src_memory);
void WriteToBroadcast(psocket_t socket, s32 size, void* src_memory);
s8 ReadFromSocket(psocket_t socket, s32 size, void* dst_memory);
s8 ReadFromBroadcast(psocket_t socket, s32 size, void* dst_memory);