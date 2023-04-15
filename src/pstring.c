#include "pstring.h"

#include <stdlib.h>

char* GetLastSection(const char* src_str, u64 len, char splitter)
{
    u64 last_splitter_location = 0;
    for (u64 i = 0; i < len; i++)
    {
        if (src_str[i] == splitter)
            last_splitter_location = i;
    }

    if (last_splitter_location == 0)
        return NULL;

    u64 new_buffer_size = len - last_splitter_location + 1;

    char* new_buffer = (char*)malloc(new_buffer_size);
    memcpy(new_buffer, src_str + last_splitter_location + 1, new_buffer_size);

    return new_buffer;
}