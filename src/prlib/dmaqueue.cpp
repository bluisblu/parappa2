#include "dmaqueue.h"

#include "prlib/dma.h"

#include <eekernel.h>

#include <malloc.h>

PrDmaQueue::PrDmaQueue(u_int size) {
    mSize  = size;
    mQueue = (PrDmaList*)memalign(128, (size + 1) * 128);

    Initialize();
    FlushCache(WRITEBACK_DCACHE);

    mStarted = false;
    mPos = 0;

    mQueue = (PrDmaList*)PR_UNCACHEDACCEL(mQueue);
}

PrDmaQueue::~PrDmaQueue() {
    if (mQueue != NULL) {
        free(PR_DECACHE(mQueue));
    }
}

void PrDmaQueue::Initialize() {
    for (int i = 0; i <= mSize; i++) {
        PrDmaList* queue = &mQueue[i];

        queue->unk0 = 0;
        queue->unk4 = 0;
        queue->unk8 = 0;
        queue->unkC = 0;
        
        queue->unk10.qwc = 1;
        queue->unk10.mark = 0;
        queue->unk10.id = 0x40; /* DMArefs */
        queue->unk10.next = (sceDmaTag*)queue;
        queue->unk10.p[0] = 0;
        queue->unk10.p[1] = 0;
        
        queue->unk20.qwc = 0;
        queue->unk20.mark = 0;
        queue->unk20.id = 0x50; /* DMAcall */
        queue->unk20.p[0] = 0;
        queue->unk20.p[1] = 0;
        
        queue->unk30.qwc = 0;
        queue->unk30.mark = 0;
        queue->unk30.id = 0x20; /* DMAnext */
        queue->unk30.next = &queue[1].unk10;
        queue->unk30.p[0] = 0;
        queue->unk30.p[1] = 0;
    }
}

void PrDmaQueue::Start() {
    mPos = false;
    PrWaitDmaFinish(SCE_DMA_VIF1);

    *D_CTRL |= 0x40; /* Perform stall control on VIF1 ch. */
    *D_STADR = (u_int)PR_DECACHE(mQueue);

    sceDmaChan* chan = sceDmaGetChan(SCE_DMA_VIF1);
    chan->chcr.TTE = 1;

    sceDmaSend(chan, PR_DECACHE(&mQueue[0].unk10));
    mStarted = true;
}

void PrDmaQueue::Append(void* tag) {
    extern bool warned;

    if (mPos == mSize) {
        if (!warned) {
        #if 0 /* (poly): Only present on McDonald's Demo build */
            printf("PRLIB(WARN): DMA queue ovewrflow\n");
            printf("PRLIB(WARN): you must initialize the queue with more size\n");
        #endif
            warned = true;
        }

        Wait();
        Start();
    }

    mQueue[mPos].unk20.next = (sceDmaTag*)tag;
    asm("sync.l");

    mPos++;
    PrDmaList* queue = &mQueue[mPos - 1];
    *D_STADR = GetNextListAddr(queue);
}

void PrDmaQueue::Wait() {
    mQueue[mPos].unk20.id = 0x70; /* DMAend */
    mQueue[mPos].unk20.next = NULL;
    asm("sync.l");

    PrDmaList* queue = &mQueue[mPos - 1];
    *D_STADR = GetNextListAddr(queue + 1);

    PrWaitDmaFinish(SCE_DMA_VIF1);
    *D_CTRL &= ~0xc0; /* Disable stall control. */

    mQueue[mPos].unk20.id = 0x50;
    mStarted = false;
}
