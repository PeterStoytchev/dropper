// Contains common communication primatives
// Padding is disabled for all structures in this file, 
// because they are meant to be sent over the wire

#pragma once

#include "types.h"

#pragma pack(push, 1)

struct file_transfer_request
{
    char sender_name[32];
    char sender_address[3 * 4]; //IPv4 of format xxx.xxx.xxx.xxx
    char file_name[64];
};

enum {
    NONE,
    OK,
    NOT_OK,
    END
} file_transfer_response;

#pragma pack(pop)