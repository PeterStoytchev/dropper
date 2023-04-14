#include <stdio.h>
#include <string.h>

#include "psocket.h"

#include "communication.h"

void reciver_entrypoint(const char* dir)
{
    printf("reciver mode\n");

    //for (;;)
    {
        printf("Open socket and wait for connection.\n");
        psocket_t server_socket = OpenSocket();
        ListenSocket(server_socket);

        psocket_t sender_socket = AcceptSocket(server_socket);

        printf("Read file request.\n");

        struct file_transfer_request tr;
        memset(&tr, 0, sizeof(tr));

        ReadFromSocket(sender_socket, sizeof(tr), &tr);

        printf("sender_name: %s\n", tr.sender_name);
        printf("sender_address: %s\n", tr.sender_address);
        printf("file_name: %s\n", tr.file_name);

        printf("Do you want to recieve this file? (Y/N)\n");

        char placeholder = 'Y';
        printf("debug: assuming yes\n");

        enum transfer_response resp;
        if (placeholder == 'Y')
        {
            printf("sending yes\n");
            resp = OK;
        }
        else 
        {
            resp = NOT_OK;
        }

        //Send our response
        WriteToSocket(sender_socket, sizeof(resp), &resp);
        if (resp == NOT_OK)
        {
            CloseSocket(sender_socket);
            return;
        }
        else if (resp == OK)
        {
            FILE* f = fopen(tr.file_name, "wb");
            ReciveFileInChuncks(f, sender_socket);
            fclose(f);

            printf("File received\n");
        }
        else
        {
            printf("fuck!\n");
        }
    }    
}

void sender_entrypoint(const char* dir)
{
    printf("sender mode\n");

    /*
    printf("do a network broadcast, to detect potential recicvers\n");
    printf("Print user for selection or s for skip and exit\n");
    
    printf("if negative answer, exit!\n");
    printf("If target selected, request transfer to it.\n");
    */
    
    psocket_t sending_socket = OpenSocketAtDestination("127.0.0.1");

    struct file_transfer_request tr = CreateRequestFromConstants("Ivan", "10.0.0.136", "test.txt");
    WriteToSocket(sending_socket, sizeof(tr), &tr);

    enum transfer_response fr;
    ReadFromSocket(sending_socket, sizeof(fr), &fr);

    if (fr == NOT_OK)
    {
        printf("Remote user declied your request!\n");
        CloseSocket(sending_socket);
        
        return;
    }

    FILE* f = fopen("test_read.txt", "rb");

    SendFileInChuncks(f, sending_socket);

    CloseSocket(sending_socket);
}

int main(int argc, char* argv[])
{
    /*
    if (argc < 3)
    {
        printf("Not enough arguments!");
        return 0;
    }
    */

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
