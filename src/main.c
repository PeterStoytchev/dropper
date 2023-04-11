#include <stdio.h>
#include <string.h>

#include "psocket.h"

void reciver_entrypoint(const char* dir)
{
    printf("reciver mode\n");

    //for (;;)
    {
        printf("Open socket and wait for connection.\n");
        psocket_t server_socket = OpenSocket();
        ListenSocket(server_socket);

        psocket_t sender_socket = AcceptSocket(server_socket);

        char mem[32];
        memset(mem, 0, sizeof(mem));

        printf("reading...\n");
        ReadFromSocket(sender_socket, sizeof(mem), mem);
        printf("mem: %s\n", mem);

        CloseSocket(sender_socket);
        CloseSocket(server_socket);

        printf("Read file request.\n");
        printf("Prompt user to accept\n");

        printf("Prompt user to accept\n");

        printf("If yes, read file and write to disk\n");

        printf("if no, continue\n");
    }    
}

void sender_entrypoint(const char* dir)
{
    printf("sender mode\n");

    printf("do a network broadcast, to detect potential recicvers\n");
    printf("Print user for selection or s for skip and exit\n");
    
    printf("if negative answer, exit!\n");
    printf("If target selected, request transfer to it.\n");
    
    psocket_t sending_socket = OpenSocketAtDestination("10.0.0.136");
    
    const char* text = "12345678910";
    char payload[32];
    memset(payload, 0, sizeof(payload));
    strcpy(payload, text);

    WriteToSocket(sending_socket, sizeof(payload), payload);

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
