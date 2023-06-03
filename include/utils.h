#pragma once

#include "types.h"

#include <stdio.h>

s64 UtilGetFileSize(FILE* f);
char* GetFileNameWitoutPath(const char* src_str, u64 len);
const char* ConcatenatePath(const char* dir, const char* file_name);