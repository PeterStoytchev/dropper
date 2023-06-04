#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"
#include "psocket.h"
#include "communication.h"

void reciver_entrypoint(const char* dir)
{
    DEBUG_LOG("Reciver mode\n");

    struct psocket broadcast_reply_socket = CreateSocket(STYPE_SERVER, PROTO_UDP, NULL, NETWORK_PORT_BROADCAST);
    struct network_discovery_request req;

    DEBUG_LOG("Stuck on reading...\n");
    u32 remote_ip = ReadFromSocket_GetIncoming(broadcast_reply_socket, sizeof(req), &req);

    CloseSocket(broadcast_reply_socket);

    char ip_str[INET_ADDRSTRLEN];
    IPtoString(remote_ip, ip_str, INET_ADDRSTRLEN);

    DEBUG_LOG("Got a broadcast from %s, calling back to %s\n", req.name, ip_str);

    {
        struct psocket sender_socket = CreateSocket(STYPE_CLIENT, PROTO_TCP, remote_ip, NETWORK_PORT);

        struct file_transfer_request tr;
        memset(&tr, 0, sizeof(tr));

        ReadFromSocket(sender_socket, sizeof(tr), &tr);

        USER_LOG("Got a file request!\n");

        USER_LOG("Sender Name: %s\n", req.name);
        USER_LOG("Sender Address: %s\n", ip_str);
        USER_LOG("File Name: %s\n", tr.file_name);
        USER_LOG("File Size: %f\n", (f32)tr.file_size / (f32)1024 / (f32)1024);

        // The function called will prompt the user and read their input
        enum transfer_response resp = GetTransferResponseFromUser();

        //Send the response
        WriteToSocket(sender_socket, sizeof(resp), &resp);

        //If we sent a no, we exit
        if (resp == TR_NOT_OK)
        {
            USER_LOG("File transfer declined!\n");

            CloseSocket(sender_socket);
        }
        else if (resp == TR_OK) //If we sent a yes, we read the file in chuncks
        {
            USER_LOG("File transfer accepted!\n");

            FILE* f;
            if (dir != NULL)
            {
                const char* final_path = ConcatenatePath(dir, tr.file_name);
                f = fopen(final_path, "wb");
                free(final_path);
            }
            else
            {
                f = fopen(tr.file_name, "wb");
            }

            ReciveFileInChuncks(f, tr.file_size, sender_socket);
            fclose(f);


            USER_LOG("File received\n");
        }
    }
}

void sender_entrypoint(const char* dir)
{
    DEBUG_LOG("Sender mode\n");

    const char* file_name = GetFileNameWitoutPath(dir, strlen(dir));
    if (file_name == NULL)
    {
        DEBUG_LOG("Couldn't extract the file name, ERROR AND EXIT!\n");
        USER_LOG("Invalid path provided!\n");

        free((void*)file_name);

        return;
    }

    struct psocket broadcast_socket = CreateSocket(STYPE_CLIENT, PROTO_UDP, NULL, NETWORK_PORT_BROADCAST);
    
    struct network_discovery_request req = CreateNetworkDiscoveryRequestFromConstants("ivan");

    // Do network broadcast, notifying recivers of our IP
    // after that, they should connect to us
    //@Note: Windows only sends brodcasts out of the 'main' network adatapter (for some reason?), so this my not find all recivers (look into using multicast instead?)
    WriteToSocket(broadcast_socket, sizeof(req), &req);
    CloseSocket(broadcast_socket);

    DEBUG_LOG("Broadcast sent, listeing for callbacks!\n");
    struct psocket server_socket = CreateSocket(STYPE_SERVER, PROTO_TCP, NULL, NETWORK_PORT);
    
    // For now, just accept the first one to connect back
    // Later, this will run for some time, before we present the user with a list
    struct psocket sending_socket = AcceptSocket(server_socket);

    // Don't need the server socket anymore
    CloseSocket(server_socket);

    FILE* f = fopen(dir, "rb");

    struct file_transfer_request tr = CreateFileTransferRequest(file_name, f);
    WriteToSocket(sending_socket, sizeof(tr), &tr); //Send the transfer request

    USER_LOG("Sent file transfer request!\n");

    enum transfer_response fr;
    ReadFromSocket(sending_socket, sizeof(fr), &fr); //Get response

    if (fr == TR_NOT_OK)
    {
        USER_LOG("Remote user declied your request!\n");
    }
    else if (fr == TR_OK)
    {
        USER_LOG("Remote user accepted your request!\nSending file!\n");
        SendFileInChuncks(f, sending_socket);
    }

    fclose(f);
    CloseSocket(sending_socket);

    free((void*)file_name);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
soSueMe:
        USER_LOG("Not enough arugments provided!\n");
        return;
    }

    if (strcmp(argv[1], "s") == 0)
    {
        if (argc < 3)
            goto soSueMe;
        
        sender_entrypoint(argv[2]);
    }
    else if (strcmp(argv[1], "r") == 0)
    {
        reciver_entrypoint(argv[2]);
    }
}