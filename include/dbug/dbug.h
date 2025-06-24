#ifndef DBUG_H
#define DBUG_H

#include <eetypes.h>

typedef struct { // 0x10
    /* 0x0 */ int debug_on;
    /* 0x4 */ int use_line;
    /* 0x8 */ int score_updown;
    /* 0xc */ int non_play;
} DBG_SELECT_STR;

#endif