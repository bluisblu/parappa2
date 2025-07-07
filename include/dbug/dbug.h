#ifndef DBUG_H
#define DBUG_H

#include <eetypes.h>

/*
 * (poly):
 * This probably isn't an actual header in source,
 * but I see it as the best way to place this.
 */
typedef struct { // 0x10
    /* 0x0 */ int debug_on;
    /* 0x4 */ int use_line;
    /* 0x8 */ int score_updown;
    /* 0xc */ int non_play;
} DBG_SELECT_STR;

#endif /* DBUG_H */
