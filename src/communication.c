#include "communication.h"

#include <string.h>

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
        ReadFromSocket(socket, sizeof(chunck), chunck);
        fwrite(chunck->data, chunck->data_chunk_usage, 1, file);
    }
    while (chunck->next_info != END);

    free(chunck);
}