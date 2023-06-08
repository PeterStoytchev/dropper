#pragma once

#include <stdio.h>


// This looks horrible, but its simple and it just works
#ifdef USER_LOG_LEVEL
    #define USER_LOG(...) printf(__VA_ARGS__)
#else
    #define USER_LOG(...)
#endif

#ifdef DEBUG_LOG_LEVEL
    #define DEBUG_LOG(...) printf("DEBUG: "); printf(__VA_ARGS__)
#else
    #define DEBUG_LOG(...)
#endif

#ifdef VERBOSE_LOG_LEVEL
    #define VERBOSE_LOG(...) printf("VERBOSE: "); printf(__VA_ARGS__)
#else
    #define VERBOSE_LOG(...)
#endif