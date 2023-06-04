// Contains common communication primatives and functions
// Padding is disabled for all structures in this file, 
// because they are meant to be sent over the wire

#pragma once

#include <stdio.h>

#include "types.h"
#include "psocket.h"

#define DATA_CHUNCK_SIZE KB(1)

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
    char name[32];
    char ipv4[4*4]; //IPv4 of format xxx.xxx.xxx.xxx
};

struct network_discovery_request CreateNetworkDiscoveryRequestFromEnv();
struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name, const char* ipv4);

#pragma pack(pop)