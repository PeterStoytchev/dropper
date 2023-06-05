// Contains common communication primatives and functions
// Padding is disabled for all structures in this file, 
// because they are meant to be sent over the wire

#pragma once

#include <stdio.h>

#include "types.h"
#include "psocket.h"

#define DATA_CHUNCK_SIZE KB(1)

//@Note: The max len of a username in Windows is 15, for Linux 20, so I set 21 to be safe
#define MAX_NAME_LENGTH 21

#pragma pack(push, 1)

struct file_transfer_request
{
    char file_name[64];
    u64 file_size;
};

struct file_transfer_request CreateFileTransferRequest(const char* file_name, FILE* f);

enum {
    TR_NONE,
    TR_OK,
    TR_NOT_OK,
    TR_END
} transfer_response;

enum transfer_response GetTransferResponseFromUser();

struct data_chunck
{
    u16 data_chunk_usage;
    enum tranfer_response next_info;
    char data[DATA_CHUNCK_SIZE];
};

void SendFileInChuncks(FILE* file, struct psocket socket);
void ReciveFileInChuncks(FILE* file, u64 total_file_size, struct psocket socket);

struct network_discovery_request
{
    char name[MAX_NAME_LENGTH];
};

struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name);
struct network_discovery_request CreateNetworkDiscoveryRequestFromUserAcc();

#pragma pack(pop)