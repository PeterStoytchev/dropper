#include "psocket.h"
#include "log.h"

#include <stdio.h>
#include <assert.h>

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


//@Note: If null is passed to ip parameter, we assume 0.0.0.0 for servers and 255.255.255.255 for clients
// Please handle input carefully, so that you don't broadcast unintentionally!
struct psocket CreateSocket(enum psocket_type type, enum psocket_proto proto, const char* ip, u16 port)
{
    assert(type != STYPE_NONE);
    assert(proto != PROTO_NONE);

    if (ip == NULL && type == STYPE_CLIENT)
    {
        assert(proto == PROTO_UDP);
    }

    struct psocket s;
    memset(&s, 0 , sizeof(s));

    StartWSA();
    
    if (proto == PROTO_TCP)
    {
        s.handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
        s.handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET; 
    address.sin_port = htons(port);

    if (ip == NULL)
    {
        if (type == STYPE_SERVER)
        {
            address.sin_addr.s_addr = INADDR_ANY;
        }
        else
        {
            address.sin_addr.s_addr = INADDR_BROADCAST;

            s32 iOptVal = 1;
            setsockopt(s.handle, SOL_SOCKET, SO_BROADCAST, &iOptVal, sizeof(s32));
        }
    }
    else
    {
        address.sin_addr.s_addr = inet_addr(ip);
    }

    s32 iResult;
    if (type == STYPE_SERVER)
    {
        iResult = bind(s.handle, (struct sockaddr*)&address, sizeof(address));

        if (proto == PROTO_TCP)
            listen(s.handle, SOMAXCONN);
    }
    else
    {
        iResult = connect(s.handle, (struct sockaddr*)&address, sizeof(address));
    }

    return s;
}

struct psocket AcceptSocket(struct psocket s)
{
    assert(s.type == STYPE_SERVER && s.proto == PROTO_TCP);

    struct psocket ret;
    ret.type = STYPE_CLIENT;
    ret.proto = PROTO_TCP;
    ret.handle = accept(s.handle, NULL, NULL);

    return ret;
}

void CloseSocket(struct psocket s)
{
    closesocket(s.handle);
}

s8 ReadFromSocket(struct psocket s, s32 size, void* dst_memory)
{
    s32 bytes_recived = 0;

    while (bytes_recived != size)
    {
        s32 iResult = recv(s.handle, (char*)dst_memory + bytes_recived, size - bytes_recived, 0);

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

void WriteToSocket(struct psocket socket, s32 size, void* src_memory)
{
    s32 iSendResult = send(socket.handle, src_memory, size, 0);

    if (iSendResult == SOCKET_ERROR) {
        VERBOSE_LOG("send failed with error: %d\n", WSAGetLastError());
        return;
    }
    VERBOSE_LOG("Bytes sent: %d\n", iSendResult);
}