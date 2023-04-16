#include "log.h"
#include "utils.h"
#include "communication.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

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
    if (c == 'Y')
    {
        return OK;
    }
        
    return NOT_OK;
}

// Stolen from: https://stackoverflow.com/a/2422723
// May be moved later to the utils file
u32 RoundClosestUp(u32 dividend, u32 divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

void ReciveFileInChuncks(FILE* file, u64 total_file_size, psocket_t socket)
{
    struct data_chunck* chunck = (struct data_chunck*)malloc(sizeof(struct data_chunck));

    u32 chunck_counter = 0;
    u32 total_chuncks = RoundClosestUp(total_file_size, DATA_CHUNCK_SIZE);

    do
    {
        ReadFromSocket(socket, sizeof(struct data_chunck), chunck);
        fwrite(chunck->data, chunck->data_chunk_usage, 1, file);

        if (++chunck_counter % 100 == 0)
        {
            USER_LOG("Progress %lu/%lu chuncks!\r", chunck_counter, total_chuncks);
        }
    }
    while (chunck->next_info != END);

    free(chunck);

    USER_LOG("Progress %lu/%lu\n", chunck_counter, total_chuncks);
    USER_LOG("Recived all chuncks!\n");
}

void SendFileInChuncks(FILE* file, psocket_t socket)
{
    struct data_chunck* chunck = (struct data_chunck*)malloc(sizeof(struct data_chunck));
    chunck->next_info = OK;

    u32 chunck_counter = 0;
    u32 total_chuncks = RoundClosestUp((u32)UtilGetFileSize(file), DATA_CHUNCK_SIZE);

    do
    {
        chunck->data_chunk_usage = (u16)fread(chunck->data, 1, DATA_CHUNCK_SIZE, file);
        
        if (chunck->data_chunk_usage != DATA_CHUNCK_SIZE)
            chunck->next_info = END;

        WriteToSocket(socket, sizeof(struct data_chunck), chunck);

        if (++chunck_counter % 100 == 0)
        {
            USER_LOG("Progress %lu/%lu chuncks!\r", chunck_counter, total_chuncks);
        }
    }
    while (chunck->next_info != END);

    free(chunck);

    USER_LOG("Progress %lu/%lu\n", chunck_counter, total_chuncks);
    USER_LOG("Sent all chuncks!\n");
}