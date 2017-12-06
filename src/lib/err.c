#include "err.h"
#include "types.h"


char *
err_getstr(status_t err_code)
{
    switch (err_code) {
        case KB_SUCCESS:
            return "Success";
        case KB_ERR_OOM:
            return "Out of memory";
        case KB_ERR_INPUT:
            return "Invalid input";
        case KB_ERR_BOUNDS:
            return "Index out of bounds";
        case KB_ERR_FULL:
            return "Object full";
        case KB_ERR_EVICT:
            return "Problem evicting";
        case KB_ERR:
        default:
            return "Unknown error";
    }
    return "";
}
