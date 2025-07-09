#include "prlib/mfifo.h"

#include <eeregs.h>

#define PR_MFIFOSIZE (0x10000)
extern char mfifoBase[PR_MFIFOSIZE] __attribute__((section(".mfifo")));

static inline u_int PrMfifoGetUnsentDataSize(u_int arg0, u_int arg1) {
    return (arg1 - arg0) & 0xfff0;
}

void PrInitializeMfifo() {
    *D_RBOR = (u_int)mfifoBase;
    *D_RBSR = PR_MFIFOSIZE - 0x10;
}

INCLUDE_ASM("prlib/mfifo", PrCleanupMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrStartMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrStopMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrWaitMfifo__Fv);

#ifndef NON_MATCHING
INCLUDE_ASM("prlib/mfifo", PrSendMfifo__FPC10_sceDmaTag);
#else
void PrSendMfifo(const sceDmaTag* tag) {
    u_int a0 = tag->qwc + 1;
    u_int a2 = a0 * 16;

    while ( *D8_CHCR & 0x100 );

    *D8_SADR = (u_int)tag & 0xfff0;
    *D8_QWC = a0;

    u_int v0;
    u_int v1;
    do {
        v1 = *D2_TADR;
        v0 = *D8_MADR;
        if (v1 == v0) {
            break;
        }
    } while (a2 >= PrMfifoGetUnsentDataSize(v0, v1));

    *D8_CHCR |= 0x100;
}
#endif

u_int PrMfifoUnsentDataSize() {
    return PrMfifoGetUnsentDataSize(*D2_TADR, *D8_MADR);
}
