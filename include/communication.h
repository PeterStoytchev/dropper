// Contains common communication primatives
// Padding is disabled for all structures in this file, 
// because they are meant to be sent over the wire

#pragma once

#include <stdio.h>

#include "types.h"
#include "psocket.h"

#pragma pack(push, 1)

struct file_transfer_request
{
    char sender_name[32];
    char sender_address[3 * 4]; //IPv4 of format xxx.xxx.xxx.xxx
    char file_name[64];
    u64 file_size;
};

struct file_transfer_request CreateRequestFromConstants(const char* sender_name, const char* sender_address, const char* file_name);

enum {
    NONE,
    OK,
    NOT_OK,
    END
} transfer_response;

#define DATA_CHUNCK_SIZE KB(1)

struct data_chunck
{
    u16 data_chunk_usage;
    enum tranfer_response next_info;
    char data[DATA_CHUNCK_SIZE];
};

void ReciveFileInChuncks(FILE* file, psocket_t socket);
void SendFileInChuncks(FILE* file, psocket_t socket);

struct network_discovery_request
{
    char name[32];
    char ipv4[3*4];
};

struct network_discovery_request CreateNetworkDiscoveryRequestFromConstants(const char* name, const char* ipv4);

//TODO: This is a utility function, and should really move somewhere else
s64 UtilGetFileSize(FILE* f);

#pragma pack(pop)