#ifndef DMAQUEUE_H
#define DMAQUEUE_H

#include "common.h"

#include <eetypes.h>
#include <libdma.h>

/* TODO(poly): Perhaps move to the common header? */
#define PR_DECACHE(addr) (void*)((u_int)(addr) & 0x0fffffff)

struct PrDmaList {
    int unk0;
    int unk4;
    int unk8;
    int unkC;

    sceDmaTag unk10;
    sceDmaTag unk20;
    sceDmaTag unk30;

    char unk40[64];
};

class PrDmaQueue {
public:
    PrDmaQueue(u_int size);
    ~PrDmaQueue();

    void Initialize();
    void Start();
    void Append(void* tag);
    void Wait();

    u_int GetNextListAddr(PrDmaList* queue) {
        return (u_int)PR_DECACHE(queue + 1);
    }

private:
    PrDmaList* mQueue;
    u_int mSize;
    bool mStarted;
    int mPos;
};

#endif /* DMAQUEUE_H */
