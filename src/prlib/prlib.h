#ifndef PRLIB_PRLIB_H
#define PRLIB_PRLIB_H

#include "common.h"

typedef enum {
    PR_FLOAT_PARAM_DISTURBANCE = 0,
    PR_DEBUG_PARAM_NUM = 1
} PrDEBUG_PARAM;

typedef union {
    int   d;
    float f;
} PrDebugParam;

#endif