#include "utils.h"

s64 UtilGetFileSize(FILE* f)
{
    s64 current_pos = ftell(f);
    fseek(f, 0L, SEEK_END);

    s64 size = ftell(f);

    fseek(f, current_pos, SEEK_SET);

    return size;
}