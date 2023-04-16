#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"
#include "psocket.h"
#include "communication.h"
#include "pstring.h"

void reciver_entrypoint(const char* dir)
{
    DEBUG_LOG("Reciver mode\n");

    psocket_t broadcast_reply_socket = OpenSocketUDPServer();
    struct network_discovery_request req;

    DEBUG_LOG("Stuck on reading...\n");
    ReadFromBroadcast(broadcast_reply_socket, sizeof(req), &req);

    CloseSocket(broadcast_reply_socket);

    DEBUG_LOG("Got a broadcast, calling back to %s\n", req.ipv4);

    {
        psocket_t sender_socket = OpenSocketAtDestination(req.ipv4);

        struct file_transfer_request tr;
        memset(&tr, 0, sizeof(tr));

        ReadFromSocket(sender_socket, sizeof(tr), &tr);

        USER_LOG("Got a file request!\n");

        USER_LOG("Sender Name: %s\n", req.name);
        USER_LOG("Sender Address: %s\n", req.ipv4);
        USER_LOG("File Name: %s\n", tr.file_name);
        USER_LOG("File Size: %f\n", (f32)tr.file_size / (f32)1024 / (f32)1024);

        // The function called will prompt the user and read their input
        enum transfer_response resp = GetTransferResponseFromUser();

        //Send the response
        WriteToSocket(sender_socket, sizeof(resp), &resp);

        //If we sent a no, we exit
        if (resp == NOT_OK)
        {
            USER_LOG("File transfer declined!\n");

            CloseSocket(sender_socket);
        }
        else if (resp == OK) //If we sent a yes, we read the file in chuncks
        {
            USER_LOG("File transfer accepted!\n");

            const char* final_path = ConcatenatePath(dir, tr.file_name);

            FILE* f = fopen(final_path, "wb");
            ReciveFileInChuncks(f, tr.file_size, sender_socket);
            fclose(f);

            free((void*)final_path);

            USER_LOG("File received\n");
        }
    }    
}

void sender_entrypoint(const char* dir)
{
    DEBUG_LOG("Sender mode\n");

    const char* file_name = GetLastSection(dir, strlen(dir), '\\');
    if (file_name == NULL)
    {
        DEBUG_LOG("Couldn't extract the file name, ERROR AND EXIT!\n");
        USER_LOG("Invalid path provided!\n");

        free((void*)file_name);

        return;
    }

    free((void*)file_name);

    psocket_t broadcast_socket = OpenSocketBroadcast();
    
    struct network_discovery_request req = CreateNetworkDiscoveryRequestFromEnv();

    // Do network broadcast, notifying recivers of our IP
    // after that, they should connect to us
    WriteToBroadcast(broadcast_socket, sizeof(req), &req);
    CloseSocket(broadcast_socket);

    DEBUG_LOG("Broadcast sent, listeing for callbacks!\n");
    psocket_t server_socket = OpenSocket();
    ListenSocket(server_socket);
    
    // For now, just accept the first one to connect back
    // Later, this will run for some time, before we present the user with a list
    psocket_t sending_socket = AcceptSocket(server_socket);

    // Don't need the server socket anymore
    CloseSocket(server_socket);

    FILE* f = fopen(dir, "rb");

    struct file_transfer_request tr = CreateFileTransferRequest(file_name, f);
    WriteToSocket(sending_socket, sizeof(tr), &tr);

    USER_LOG("Sent file transfer request!\n");

    enum transfer_response fr;
    ReadFromSocket(sending_socket, sizeof(fr), &fr);

    if (fr == NOT_OK)
    {
        USER_LOG("Remote user declied your request!\n");
    }
    else if (fr == OK)
    {
        USER_LOG("Remote user accepted your request!\nSending file!\n");
        SendFileInChuncks(f, sending_socket);
    }

    fclose(f);
    CloseSocket(sending_socket);
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