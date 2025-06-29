#ifndef PRLIB_RENDERSTUFF_H
#define PRLIB_RENDERSTUFF_H

#include "common.h"

#include <eestruct.h>

class PrRenderStuff {
public:
    void Initialize(sceGsZbuf zbuf);
    void Cleanup();

private:
    PR_PADDING(unk0, 0x10);
};

extern PrRenderStuff prRenderStuff;

#endif