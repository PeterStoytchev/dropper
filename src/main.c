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

        enum file_transfer_response resp;
        if (placeholder == "Y")
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
            printf("Rread file and write to disk\n");
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
    
    const char* text = "12345678910";

    WriteToSocket(sending_socket, 12, text);

    CloseSocket(sending_socket);

    printf("If no, exit!\n");
    printf("If yes, read the file and send it\n");
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
