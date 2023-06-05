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

struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name)
{
    struct network_discovery_request req;
    
    strcpy(req.name, name);

    return req;
}

struct network_discovery_request CreateNetworkDiscoveryRequestFromUserAcc()
{
    struct network_discovery_request ndr;
    u32 bufCharCount = MAX_NAME_LENGTH;

    memset(&ndr, 0, sizeof(ndr));

    GetUserName(ndr.name, &bufCharCount);
    
    return ndr;
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
        return TR_OK;
    }
        
    return TR_NOT_OK;
}

// Stolen from: https://stackoverflow.com/a/2422723
// May be moved later to the utils file
u32 RoundClosestUp(u32 dividend, u32 divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

void ReciveFileInChuncks(FILE* file, u64 total_file_size, struct psocket socket)
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
    while (chunck->next_info != TR_END);

    free(chunck);

    USER_LOG("Progress %lu/%lu\n", chunck_counter, total_chuncks);
    USER_LOG("Recived all chuncks!\n");
}

void SendFileInChuncks(FILE* file, struct psocket socket)
{
    struct data_chunck* chunck = (struct data_chunck*)malloc(sizeof(struct data_chunck));
    chunck->next_info = TR_OK;

    u32 chunck_counter = 0;
    u32 total_chuncks = RoundClosestUp((u32)UtilGetFileSize(file), DATA_CHUNCK_SIZE);

    do
    {
        chunck->data_chunk_usage = (u16)fread(chunck->data, 1, DATA_CHUNCK_SIZE, file);
        
        if (chunck->data_chunk_usage != DATA_CHUNCK_SIZE)
            chunck->next_info = TR_END;

        WriteToSocket(socket, sizeof(struct data_chunck), chunck);

        if (++chunck_counter % 100 == 0)
        {
            USER_LOG("Progress %lu/%lu chuncks!\r", chunck_counter, total_chuncks);
        }
    }
    while (chunck->next_info != TR_END);

    free(chunck);

    USER_LOG("Progress %lu/%lu\n", chunck_counter, total_chuncks);
    USER_LOG("Sent all chuncks!\n");
}