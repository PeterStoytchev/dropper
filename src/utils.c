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