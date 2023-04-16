#include "log.h"
#include "communication.h"

#include <string.h>
#include <stdlib.h>

struct file_transfer_request CreateFileTransferRequest(const char* file_name, FILE* f)
{
    struct file_transfer_request tr;
    memset(&tr, 0, sizeof(tr));

    strcpy(tr.file_name, file_name);
    tr.file_size = UtilGetFileSize(f);

    return tr;
}

struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name, const char* ipv4)
{
    struct network_discovery_request req;
    
    strcpy(req.name, name);
    strcpy(req.ipv4, ipv4);

    return req;
}

struct network_discovery_request CreateNetworkDiscoveryRequestFromEnv()
{
    // Get the PC's hostname
    char host[256];
    memset(host, 0, sizeof(host));
    gethostname(host, sizeof(host));

    // Get the PC's IP
    struct hostent *host_entry = gethostbyname(host);
    char *IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    // This shouldn't cause memory issues, since we call the next function, before we return
    return CreateNetworkDiscoveryRequestFromConstants(host, IP);
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