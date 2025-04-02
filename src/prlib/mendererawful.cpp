#include "common.h"

#include "prlib/dma.h"
#include "prlib/random.h"

#include <eekernel.h>
#include <eetypes.h>
#include <libdma.h>
#include <libgraph.h>

/* data */
extern u_long mendererFadeData[7][2];

/* sbss */
extern u_int mendererAwfulColor;
extern u_int awfulChangeTimer;

void SetNextSwitchRotationTimer() {
    awfulChangeTimer = (u_int)((PrFloatRandom() * 4.0f + 3.0f) * 60.0f);
}

INCLUDE_ASM("prlib/mendererawful", GetAwfulRotation__Fv);

INCLUDE_ASM("prlib/mendererawful", PrStartAwfulRotation__Fv);

void PrFadeFrameImage(float arg0) {
    if (arg0 == 0.0f) {
        return;
    }

    u_int alp = (u_int)(arg0 * 128.0f + 0.5f);
    mendererFadeData[3][0] =
        SCE_GS_SET_RGBAQ(
            (mendererAwfulColor >> 0 ) & 255,
            (mendererAwfulColor >> 8 ) & 255,
            (mendererAwfulColor >> 16) & 255,
            alp & 255,
            0
        );

    sceDmaChan *chan = sceDmaGetChan(SCE_DMA_GIF);
    chan->chcr.TTE = 0;

    FlushCache(0);
    PrWaitDmaFinish(SCE_DMA_GIF);
    sceDmaSendN(chan, &mendererFadeData, 7);
}

INCLUDE_ASM("prlib/mendererawful", PrInitializeAwfulBackground__FPv);

INCLUDE_ASM("prlib/mendererawful", PrDrawAwfulBackground__FG10sceGsFrame);

INCLUDE_ASM("prlib/mendererawful", PrUpdateAwfulMenderer__Fv);

/* nalib/navector.h */
INCLUDE_ASM("prlib/mendererawful", func_00150CE8);

INCLUDE_ASM("prlib/mendererawful", func_00150D10);
