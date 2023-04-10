#include <stdio.h>
#include <string.h>

void reciver_entrypoint(const char* dir)
{
    printf("reciver mode\n");

    //for (;;)
    {
        printf("Open socket and wait for connection.\n");
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
    
    pritnf("if negative answer, exit!\n");
    printf("If target selected, request transfer to it.\n");
    
    printf("If no, exit!\n");
    printf("If yes, read the file and send it\n");
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Not enough arguments!");
        return 0;
    }

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
