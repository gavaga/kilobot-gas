#ifndef __ERR_H__
#define __ERR_H__

#include "debug.h"

char *err_getstr(status_t err_code);

#define ASSERT(cond, fmt, args...) \
    do { \
        if (!(cond)) { \
            DEBUG_PRINT(fmt, ##args); \
            assert(cond); \
        } \
    } while(0)

#define ASSERT_OR_GOTO(cond, target, fmt, args...) \
    do { \
        if (!(cond)) { \
            DEBUG_PRINT(fmt, ##args); \
            goto target; \
        } \
    } while(0)

#define ASSERT_OR_ERR(cond, tgt, errcode) \
    ASSERT_OR_GOTO(cond, tgt, "%s\n", err_getstr((errcode)))

#define ASSERT_OR_RET_ERR(st_var, cond, errcode) \
    do { \
        if (!(cond)) { \
            st_var = errcode; \
            DEBUG_PRINT("%s\n", err_getstr(errcode)); \
            goto err; \
        } \
    } while(0)

#define SUCCESS_OR_PROP_ERR(st_var, expr) \
    do { \
        status_t tmp = (expr); \
        if (tmp != KB_SUCCESS) { \
            st_var = tmp; \
            goto err; \
        } \
    } while(0)

#define ERR_STRING(errtype)     err_getstr(errtype)

// status values types
#define KB_ERR        -0x1
#define KB_SUCCESS    0x0
#define KB_ERR_OOM    0x1
#define KB_ERR_INPUT  0x2
#define KB_ERR_BOUNDS 0x3
#define KB_ERR_FULL   0x4
#define KB_ERR_EVICT  0x5

#endif
