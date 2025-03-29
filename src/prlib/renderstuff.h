#ifndef PRLIB_RENDERSTUFF_H
#define PRLIB_RENDERSTUFF_H

#include <eestruct.h>

class PrRenderStuff {
public:
    void Initialize(sceGsZbuf zbuf);
    void Cleanup();
private:
    char unk0[0x10];
};

extern PrRenderStuff prRenderStuff;

#endif