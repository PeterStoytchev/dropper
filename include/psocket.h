#include "types.h"

#define OS_WINDOWS

#ifdef OS_WINDOWS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define NETWORK_PORT "13337"

typedef SOCKET psocket_t;

#else

typedef s32 psocket_t;

#define NETWORK_PORT 13337

#endif


psocket_t OpenSocket();
psocket_t OpenSocketAtDestination(const char* dst);
void CloseSocket(psocket_t socket);

void ListenSocket(psocket_t socket);
psocket_t AcceptSocket(psocket_t server_socket);

void WriteToSocket(psocket_t socket, s32 size, void* src_memory);
void ReadFromSocket(psocket_t socket, s32 size, void* dst_memory);