#include "common.h"

#include <intrman.h>
#include <loadcore.h>
#include <sif.h>
#include <sifrpc.h>
#include <thread.h>

#include <stdio.h>

/* bgm_com.c */
extern int sce_bgm_loop(void);

IOP_INCLUDE_RODATA("wp2cd/nonmatchings/iop/bgm_entr", D_000039D0);

int start(void) {
    struct ThreadParam param;
    int th;

    CpuEnableIntr();

    if (!sceSifCheckInit()) {
        sceSifInit();
    }

    sceSifInitRpc(0);

    printf("WaveP2 driver version 1.2.0\n");

    param.attr         = TH_C;
    param.entry        = sce_bgm_loop;
    param.initPriority = 0x57;
    param.stackSize    = 0x800;
    param.option       = 0;

    th = CreateThread(&param);
    if (th > 0) {
        StartThread(th, NULL);
        printf(" WaveP2 loader thread \n");
        return RESIDENT_END;
    } else {
        return NO_RESIDENT_END;
    }
}
