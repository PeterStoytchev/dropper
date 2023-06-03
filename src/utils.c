#include "utils.h"

#include "communication.h"

#include <math.h>
#include <stdlib.h>

// This is temporary
#define OS_WINDOWS


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
    u64 dir_len = strlen(dir);
    u64 file_name_len = strlen(file_name);

    u64 storage_len = dir_len + file_name_len + 1;
    char* storage = (char*)malloc(storage_len);
    memset(storage, 0, storage_len);

    memcpy(storage, dir, dir_len);
    
    // This should be temporary
    #ifdef OS_WINDOWS
        storage[dir_len] = '\\';
    #else
        storage[dir_len] = '/';
    #endif

    memcpy(storage + dir_len + 1, file_name, file_name_len);

    return storage;
}

char* GetFileNameWitoutPath(const char* src_str, u64 len)
{
    #ifdef OS_WINDOWS
        char splitter = '\\';
    #else
        char splitter = '/';
    #endif

    for (u64 i = len - 1; i != 0; i--)
    {
        if (src_str[i] == splitter)
        {
            if (i == 0)
                return NULL;

            u64 new_buffer_size = len - i + 1;

            char* new_buffer = (char*)malloc(new_buffer_size);
            memcpy(new_buffer, src_str + i + 1, new_buffer_size);
        
            return new_buffer;
        }
    }
}