#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#include "types.h"

#ifndef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define DEBUG_PRINT(fmt, args...) \
    do { \
        if (DEBUG_TEST) { \
            fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, \
                    __LINE__, __FUNCTION__, ##args); \
        } \
    } while(0)

#endif
