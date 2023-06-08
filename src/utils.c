#include "utils.h"

#include "communication.h"

#include <math.h>
#include <stdlib.h>

s64 UtilGetFileSize(FILE* f)
{
    s32 current_pos = ftell(f);
    fseek(f, 0L, SEEK_END);

    s64 size = ftell(f);

    fseek(f, current_pos, SEEK_SET);

    return size;
}

const char* ConcatenatePath(const char* dir, const char* file_name)
{
    u64 storage_len = strlen(dir) + strlen(file_name) + 1;
    char* storage = (char*)malloc(storage_len);
    memset(storage, 0, storage_len);

    sprintf(storage, "%s%c%s\n", dir, PATH_CHAR, file_name);

    return storage;
}

char* GetFileNameWitoutPath(const char* src_str, u64 len)
{
    u64 loc = 0;
    for (u64 i = len - 1; i < -1; i--)
    {
        if (src_str[i] == PATH_CHAR)
        {
            if (i == (len - 1))
                return NULL;

            loc = i + 1;
            break;
        }
    }

    u64 new_buffer_size = len - loc + 1;

    char* new_buffer = (char*)malloc(new_buffer_size);
    memcpy(new_buffer, src_str + loc, new_buffer_size);

    return new_buffer;
}