#include "dmaqueue.h"

#include <eekernel.h>

#include <malloc.h>

#define PR_DECACHE(addr) ((u_int)(addr) & 0x0fffffff)

PrDmaQueue::PrDmaQueue(u_int size) {
    this->unk4 = size;
    this->unk0 = (PrDmaList*)memalign(128, (size + 1) * 128);

    Initialize();
    FlushCache(0);

    this->unk8 = 0;
    this->unkC = 0;

    this->unk0 = (PrDmaList*)PR_UNCACHEDACCEL(this->unk0);
}

PrDmaQueue::~PrDmaQueue() {
    if (this->unk0 != NULL) {
        free((void*)PR_DECACHE(this->unk0));
    }
}

// Different compiler matches without any of the volatile trickery
// https://decomp.me/scratch/PcirV
void PrDmaQueue::Initialize() {
    u_int i;
    PrDmaList *temp_v1;
    volatile PrDmaList *var_v1;

    for (i = 0; i <= this->unk4; i++) {
        temp_v1 = (PrDmaList*)&this->unk0[i];

        var_v1 = temp_v1;
        var_v1->unk30.next = (sceDmaTag*)&var_v1[1].unk10;

        var_v1->unk0 = 0;
        var_v1->unk4 = 0;
        var_v1->unk8 = 0;
        var_v1->unkC = 0;

        var_v1->unk10.qwc = 1;
        var_v1->unk10.mark = 0;
        var_v1->unk10.id = 0x40;
        var_v1->unk10.next = (sceDmaTag*)var_v1;
        var_v1->unk10.p[0] = 0;
        var_v1->unk10.p[1] = 0;

        var_v1->unk20.qwc = 0;
        var_v1->unk20.mark = 0;
        var_v1->unk20.id = 0x50;
        var_v1->unk20.p[0] = 0;
        var_v1->unk20.p[1] = 0;

        var_v1->unk30.qwc = 0;
        var_v1->unk30.mark = 0;
        var_v1->unk30.id = 0x20;
        var_v1->unk30.p[0] = 0;
        temp_v1->unk30.p[1] = 0;
    }
}

// https://decomp.me/scratch/tVeBn
INCLUDE_ASM("prlib/dmaqueue", Start__10PrDmaQueue);

// https://decomp.me/scratch/yrbTv
INCLUDE_ASM("prlib/dmaqueue", Append__10PrDmaQueuePv);

// https://decomp.me/scratch/feA56
INCLUDE_ASM("prlib/dmaqueue", Wait__10PrDmaQueue);
