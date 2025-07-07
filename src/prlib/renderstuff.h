#ifndef PRLIB_RENDERSTUFF_H
#define PRLIB_RENDERSTUFF_H

#include "common.h"

#include <eetypes.h>
#include <eestruct.h>

class PrRenderStuff {
public:
    void Initialize(sceGsZbuf zbuf);
    void Cleanup();

private:
    PR_PADDING(unk0, 0x1c);
    int unk1C;
    sceGsZbuf unk20;
    int unk28;
};

extern PrRenderStuff prRenderStuff;

#endif /* PRLIB_RENDERSTUFF_H */
