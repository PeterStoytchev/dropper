#include "log.h"
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

struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name, const char* ipv4)
{
    struct network_discovery_request req;
    
    strcpy(req.name, name);
    strcpy(req.ipv4, ipv4);

    return req;
}

enum transfer_response GetTransferResponseFromUser()
{
    USER_LOG("Do you want to recieve this file? (Y/N)\n");

    //Read answer to promt
    char c;
    scanf(" %c", &c);
    
    //Set the correct response
    enum transfer_response resp;
    if (c == 'Y')
    {
        return OK;
    }
        
    return NOT_OK;
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

    DEBUG_LOG("Recived all chuncks!\n");
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

    DEBUG_LOG("Sent all chuncks!\n");
}