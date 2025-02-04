#ifndef DMAQUEUE_H
#define DMAQUEUE_H

#include "common.h"

#include <eetypes.h>
#include <libdma.h>

struct PrDmaList
{
    int unk0;
    int unk4;
    int unk8;
    int unkC;

    sceDmaTag unk10;
    sceDmaTag unk20;
    sceDmaTag unk30;

    char unk40[64];
};

class PrDmaQueue
{
public:
    PrDmaQueue(u_int size);
    ~PrDmaQueue();

    void Initialize();
private:
    PrDmaList* unk0;
    int unk4;
    int unk8;
    int unkC;
};

#endif