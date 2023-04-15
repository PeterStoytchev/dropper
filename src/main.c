#include <stdio.h>
#include <string.h>

#include "psocket.h"
#include "communication.h"
#include "pstring.h"

void reciver_entrypoint(const char* dir)
{
    printf("reciver mode\n");

    psocket_t broadcast_reply_socket = OpenSocketUDPServer();
    struct network_discovery_request req;

    printf("stuck on reading...\n");
    ReadFromBroadcast(broadcast_reply_socket, sizeof(req), &req);

    CloseSocket(broadcast_reply_socket);

    printf("got a broadcast, calling back to %s\n", req.ipv4);

    {
        psocket_t sender_socket = OpenSocketAtDestination(req.ipv4);

        printf("Read file request.\n");

        struct file_transfer_request tr;
        memset(&tr, 0, sizeof(tr));

        ReadFromSocket(sender_socket, sizeof(tr), &tr);

        printf("sender_name: %s\n", tr.sender_name);
        printf("sender_address: %s\n", tr.sender_address);
        printf("file_name: %s\n", tr.file_name);
        printf("file_size: %llu\n", tr.file_size);

        printf("Do you want to recieve this file? (Y/N)\n");

        //Read answer to promt
        char c;
        scanf(" %c", &c);

        //Set the correct response
        enum transfer_response resp;
        if (c == 'Y')
        {
            resp = OK;
        }
        else 
        {
            resp = NOT_OK;
        }

        //Send the response
        WriteToSocket(sender_socket, sizeof(resp), &resp);

        //If we sent a no, we exit
        if (resp == NOT_OK)
        {
            printf("File transfer declined!\n");

            CloseSocket(sender_socket);
            return;
        }
        else if (resp == OK) //If we sent a yes, we read the file in chuncks
        {
            printf("File transfer accepted!\n");

            FILE* f = fopen(tr.file_name, "wb");
            ReciveFileInChuncks(f, sender_socket);
            fclose(f);

            printf("File received\n");
        }
    }    
}

void sender_entrypoint(const char* dir)
{
    printf("sender mode\n");

    // Do network broadcast, notifying recivers of our IP
    // after that, they should connect to us
    psocket_t broadcast_socket = OpenSocketBroadcast();
    
    //hardcore the vals for now
    struct network_discovery_request req = CreateNetworkDiscoveryRequestFromConstants("desktop", "10.0.0.136");

    WriteToBroadcast(broadcast_socket, sizeof(req), &req);
    CloseSocket(broadcast_socket);

    printf("broadcast sent, listeing for callbacks!\n");
    psocket_t server_socket = OpenSocket();
    ListenSocket(server_socket);
    
    // For now, just accept the first one to connect back
    // Later, this will run for some time, before we present the user with a list
    psocket_t sending_socket = AcceptSocket(server_socket);

    FILE* f = fopen(dir, "rb");

    const char* file_name = GetLastSection(dir, strlen(dir), '\\');
    if (file_name == NULL)
    {
        printf("Got null\n");
        file_name = dir;
    }

    struct file_transfer_request tr = CreateRequestFromConstants("Ivan", "10.0.0.136", file_name);
    tr.file_size = UtilGetFileSize(f);

    WriteToSocket(sending_socket, sizeof(tr), &tr);

    printf("Sent file transfer request!\n");

    enum transfer_response fr;
    ReadFromSocket(sending_socket, sizeof(fr), &fr);

    if (fr == NOT_OK)
    {
        printf("Remote user declied your request!\n");
    }
    else if (fr == OK)
    {
        printf("Remote user accepted your request!\nSending file!\n");
        SendFileInChuncks(f, sending_socket);
    }

    fclose(f);
    CloseSocket(sending_socket);
}

int main(int argc, char* argv[])
{
    if (strcmp(argv[1], "s") == 0)
    {
        sender_entrypoint(argv[2]);
    }
    else if (strcmp(argv[1], "r") == 0)
    {
        reciver_entrypoint(argv[2]);
    }
    else
    {
        printf("fuck\n");
    }
}
