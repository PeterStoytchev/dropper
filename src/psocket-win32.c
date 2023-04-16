#include "psocket.h"
#include "log.h"

#include <stdio.h>

// A lot of copy-pasta from: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-server-code
// and: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code

// Not good practice, but it will do
static s8 isWSAStarted = 0;

void StartWSA()
{
    if (isWSAStarted)
        return;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    isWSAStarted = 1;
}

psocket_t OpenSocket()
{
    StartWSA();

    struct addrinfo *result = NULL;

    //Not very flexible, but this abstraction layer is just for this app
    //To be changed if needed
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // iResult the server address and port
    s32 iResult = getaddrinfo(NULL, NETWORK_PORT, &hints, &result);

    // Create a SOCKET for the server to listen for client connections.
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    //Bind the socket
    iResult = bind(ListenSocket, result->ai_addr, (s32)result->ai_addrlen);

    freeaddrinfo(result);

    return ListenSocket;
}

psocket_t OpenSocketAtDestination(const char* dst)
{
    StartWSA();
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* result = NULL;
    getaddrinfo(dst, NETWORK_PORT, &hints, &result);

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    connect(ConnectSocket, result->ai_addr, (s32)result->ai_addrlen);
    freeaddrinfo(result);

    return ConnectSocket;
}

psocket_t OpenSocketUDPServer()
{
    StartWSA();

    struct addrinfo *result = NULL;

    //Not very flexible, but this abstraction layer is just for this app
    //To be changed if needed
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    // iResult the server address and port
    s32 iResult = getaddrinfo(NULL, NETWORK_PORT_BROADCAST, &hints, &result);

    // Create a SOCKET for the server to listen for client connections.
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    //Bind the socket
    iResult = bind(ListenSocket, result->ai_addr, (s32)result->ai_addrlen);

    freeaddrinfo(result);

    return ListenSocket;
}

psocket_t OpenSocketBroadcast()
{
    StartWSA();

    SOCKET ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);

    s32 iOptVal = 1;
    setsockopt(ListenSocket, SOL_SOCKET, SO_BROADCAST, &iOptVal, sizeof(s32));

    return ListenSocket;
}


void ListenSocket(psocket_t socket)
{
    listen(socket, SOMAXCONN);
}

psocket_t AcceptSocket(psocket_t server_socket)
{
    return accept(server_socket, NULL, NULL);
}

void CloseSocket(psocket_t socket)
{
    closesocket(socket);
}

s8 ReadFromSocket(psocket_t socket, s32 size, void* dst_memory)
{
    s32 bytes_recived = 0;

    while (bytes_recived != size)
    {
        s32 iResult = recv(socket, (char*)dst_memory + bytes_recived, size - bytes_recived, 0);

        if (iResult > 0 )
        {
            bytes_recived += iResult;
            VERBOSE_LOG("Bytes received: %d\n", iResult);

            return 1;
        }
        else if (iResult == 0)
        {
            VERBOSE_LOG("Connection closed\n");
            return -1;
        }
        else
        {
            VERBOSE_LOG("recv failed with error: %d\n", WSAGetLastError());
            return -1;
        }
    }

    return 1;
}

s8 ReadFromBroadcast(psocket_t socket, s32 size, void* dst_memory)
{
    s32 bytes_recived = 0;

    struct sockaddr_storage their_addr;
    s32 addr_len = sizeof(their_addr);

    while (bytes_recived != size)
    {
        s32 iResult = recvfrom(socket, (char*)dst_memory + bytes_recived, size - bytes_recived, 0, (struct sockaddr *)&their_addr, &addr_len);

        if (iResult > 0 )
        {
            bytes_recived += iResult;
            VERBOSE_LOG("Bytes received: %d\n", iResult);

            return 1;
        }
        else if (iResult == 0)
        {
            VERBOSE_LOG("Connection closed\n");
            return -1;
        }
        else
        {
            VERBOSE_LOG("recv failed with error: %d\n", WSAGetLastError());
            return -1;
        }
    }

    return 1;
}


void WriteToSocket(psocket_t socket, s32 size, void* src_memory)
{
    s32 iSendResult = send(socket, src_memory, size, 0);

    if (iSendResult == SOCKET_ERROR) {
        VERBOSE_LOG("send failed with error: %d\n", WSAGetLastError());
        return;
    }
    VERBOSE_LOG("Bytes sent: %d\n", iSendResult);
}

void WriteToBroadcast(psocket_t socket, s32 size, void* src_memory)
{
    struct sockaddr_in to;
    to.sin_family = AF_INET;
    to.sin_port = htons(13338);
    to.sin_addr.s_addr = inet_addr("255.255.255.255");

    s32 iSendResult = sendto(socket, src_memory, size, 0, (SOCKADDR*)&to, sizeof(to));

    if (iSendResult == SOCKET_ERROR) {
        VERBOSE_LOG("send failed with error: %d\n", WSAGetLastError());
        return;
    }
    VERBOSE_LOG("Bytes sent: %d\n", iSendResult);
}