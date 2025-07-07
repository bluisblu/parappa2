#include "prlib/renderstuff.h"

#include "prlib/gifreg.h"
#include "prlib/mfifo.h"
#include "prlib/microprogram.h"

#include <eeregs.h>

INCLUDE_ASM("prlib/renderstuff", __13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", _$_13PrRenderStuff);

void PrRenderStuff::Initialize(sceGsZbuf zbuf) {
    this->unk20 = zbuf;
    this->unk1C = 0;
    this->unk28 = 0;

    PrLoadMicroPrograms();

    PrInitializeDmaStripGifRegister(zbuf);
    PrInitializeMfifo();

    /*
     * Disable VIF DMAtag mismatch errors to
     * avoid HW bug where these are triggered
     * with valid packets, causing stalls.
     *
     * Though for VIF0 this bit is guaranteed
     * to be set from the sceDevVif0Reset() call
     * on InitSystem() (os/system.c).
     */
    *VIF0_ERR |= (1<<1);
    *VIF1_ERR |= (1<<1);
}

INCLUDE_ASM("prlib/renderstuff", Cleanup__13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", GetZbufBits__C13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", ResetStatistics__13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", StartRender__13PrRenderStuffP13PrSceneObject);

INCLUDE_ASM("prlib/renderstuff", WaitRender__13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", AllocateTransmitDmaArray__13PrRenderStuffUi);

INCLUDE_ASM("prlib/renderstuff", AppendTransmitDmaTag__13PrRenderStuffPC10_sceDmaTagUif);

INCLUDE_ASM("prlib/renderstuff", CompareFunction__13PrRenderStuffPCvT1);

INCLUDE_ASM("prlib/renderstuff", SortTransmitDmaArray__13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", MergeRender__13PrRenderStuff);

INCLUDE_ASM("prlib/renderstuff", _GLOBAL_$D$prRenderStuff);

INCLUDE_ASM("prlib/renderstuff", _GLOBAL_$I$prRenderStuff);

/* prlib/renderstuff.h */
INCLUDE_ASM("prlib/renderstuff", func_00141650);
