#include "psocket.h"

// A lot of copy-pasta from: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-server-code
// and: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code

void StartWSA()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
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
    s32 iResult = getaddrinfo(dst, NETWORK_PORT, &hints, &result);

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    connect(ConnectSocket, result->ai_addr, (s32)result->ai_addrlen);
    freeaddrinfo(result);

    return ConnectSocket;
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

void ReadFromSocket(psocket_t socket, s32 size, void* dst_memory)
{
    s32 bytes_recived = 0;
    s32 iResult = -1;

    while (bytes_recived != size)
    {
        s32 iResult = recv(socket, (u8*)dst_memory + bytes_recived, size - bytes_recived, 0);

        if ( iResult > 0 )
        {
            printf("Bytes received: %d\n", iResult);
            bytes_recived += iResult;
        }
        else if ( iResult == 0 )
        {
            printf("Connection closed\n");
            return;
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            return;
        }
    }
}

void WriteToSocket(psocket_t socket, s32 size, void* src_memory)
{
    s32 iSendResult = send(socket, src_memory, size, 0);

    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        return;
    }
    printf("Bytes sent: %d\n", iSendResult);
}