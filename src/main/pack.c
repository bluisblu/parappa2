#include "main/pack.h"

u_int PackGetDecodeSize(u_char *fp_r) {
    return *(u_int*)fp_r;
}

INCLUDE_ASM("main/pack", PackDecode);

#ifndef NON_MATCHING
INCLUDE_ASM("main/pack", PackGetAdrs);
#else
int PackGetAdrs(/* a0 4 */ u_int adrs, /* a1 5 */ int num)
{
    if (PACK(adrs)->pack_header.fcnt <= num)
        return -1;
}
#endif

INCLUDE_ASM("main/pack", PackDbgList);
