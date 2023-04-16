#pragma once

#include "types.h"

#include <stdio.h>

s64 UtilGetFileSize(FILE* f);
const char* ConcatenatePath(const char* dir, const char* file_name);