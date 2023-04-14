#include "communication.h"

#include <string.h>
#include <stdlib.h>

struct file_transfer_request CreateRequestFromConstants(const char* sender_name, const char* sender_address, const char* file_name)
{
    struct file_transfer_request tr;
    memset(&tr, 0, sizeof(tr));

    strcpy(tr.sender_name, sender_name);
    strcpy(tr.sender_address, sender_address);
    strcpy(tr.file_name, file_name);

    return tr;
}

void ReciveFileInChuncks(FILE* file, psocket_t socket)
{
    struct data_chunck* chunck = (struct data_chunck*)malloc(sizeof(struct data_chunck));

    do
    {
        ReadFromSocket(socket, sizeof(struct data_chunck), chunck);
        fwrite(chunck->data, chunck->data_chunk_usage, 1, file);
    }
    while (chunck->next_info != END);

    free(chunck);

    printf("Recived all chuncks!\n");
}

void SendFileInChuncks(FILE* file, psocket_t socket)
{
    struct data_chunck* chunck = (struct data_chunck*)malloc(sizeof(struct data_chunck));
    chunck->next_info = OK;

    do
    {
        chunck->data_chunk_usage = fread(chunck->data, 1, DATA_CHUNCK_SIZE, file);
        
        if (chunck->data_chunk_usage != DATA_CHUNCK_SIZE)
            chunck->next_info = END;

        WriteToSocket(socket, sizeof(struct data_chunck), chunck);
    }
    while (chunck->next_info != END);

    free(chunck);

    printf("Sent all chuncks!\n");
}