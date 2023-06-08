#include "log.h"
#include "utils.h"
#include "threading.h"

#include "communication.h"

#include <math.h>
#include <stdlib.h>

#ifdef OS_WINDOWS
    #include <windows.h>
#endif

s64 UtilGetFileSize(FILE* f)
{
    s32 current_pos = ftell(f);
    fseek(f, 0L, SEEK_END);

    s64 size = ftell(f);

    fseek(f, current_pos, SEEK_SET);

    return size;
}

const char* ConcatenatePath(const char* dir, const char* file_name)
{
    u64 storage_len = strlen(dir) + strlen(file_name) + 1;
    char* storage = (char*)malloc(storage_len);
    memset(storage, 0, storage_len);

    sprintf(storage, "%s%c%s\n", dir, PATH_CHAR, file_name);

    return storage;
}

char* GetFileNameWitoutPath(const char* src_str, u64 len)
{
    u64 loc = 0;
    for (u64 i = len - 1; i < -1; i--)
    {
        if (src_str[i] == PATH_CHAR)
        {
            if (i == (len - 1))
                return NULL;

            loc = i + 1;
            break;
        }
    }

    u64 new_buffer_size = len - loc + 1;

    char* new_buffer = (char*)malloc(new_buffer_size);
    memcpy(new_buffer, src_str + loc, new_buffer_size);

    return new_buffer;
}


enum transfer_response GetTransferResponseFromUser()
{
    USER_LOG("Do you want to recieve this file? (Y/N)\n");

    //Read answer to promt
    char c;
    scanf(" %c", &c);
    
    //Set the correct response
    if (c == 'Y')
    {
        return TR_OK;
    }
        
    return TR_NOT_OK;
}

struct recvrs_handler_data
{
    u8 start;
    struct psocket server_socket;
    struct psocket sockets[PSOCKET_MAX_SOCKET_SELECTS];
    struct network_discovery_request reqs[PSOCKET_MAX_SOCKET_SELECTS];
};

void handle_recvrs(struct recvrs_handler_data* hd)
{
    Sleep(250);
    for (;;)
    {
        SetSocketBlocking(hd->server_socket, 0);

        struct psocket sock = AcceptSocket(hd->server_socket);

        SetSocketBlocking(hd->server_socket, 1);

        if (IsSocketHandleValid(sock))
        {
            hd->sockets[hd->start] = sock;
            struct network_discovery_request* req = hd->reqs + hd->start;
            ReadFromSocket(hd->sockets[hd->start], sizeof(struct network_discovery_request), req);
            
            hd->start++;
    
            if ((hd->start + 1) == PSOCKET_MAX_SOCKET_SELECTS)
                break;
        }
        else
        {
            break;
        }
    }
}

void SocketCleanup(struct psocket* sockets, u32 except_index)
{
    for (s32 i = 0; i < PSOCKET_MAX_SOCKET_SELECTS; i++)
    {
        if (i != except_index)
        {
            CloseSocket(sockets[i]);
        }
    }

}

struct psocket GetSocketSelectionFromUser(struct psocket server_socket)
{
    struct recvrs_handler_data hd;

    memset(&hd, 0, sizeof(hd));
    hd.server_socket = server_socket;

    USER_LOG("Looking for recivers!\n");

    for (;;)
    {
        handle_recvrs(&hd);

        USER_LOG("Pick a socket by entering its number (0 for refresh, -1 for exit).\n");

        s32 num_sockets = 0;
        for (; num_sockets < PSOCKET_MAX_SOCKET_SELECTS; num_sockets++)
        {
            if (hd.sockets[num_sockets].handle != 0)
            {
                USER_LOG("%i) %s\n", num_sockets + 1, hd.reqs[num_sockets].name);
            }
            else
            {
                break;
            }
        }

        s32 c;
        scanf(" %i", &c);

        if (c == 0)
        {
            continue;
        }
        else if (c == -1)
        {
            USER_LOG("Exiting!\n");
            exit(0);
        }
        else if (c > 0 && c < PSOCKET_MAX_SOCKET_SELECTS)
        {
            SocketCleanup(&hd.sockets, c - 1);
            return hd.sockets[c - 1];
        }
        else
        {
            USER_LOG("Invalid input, try again!\n");
            continue;
        }
    }
}