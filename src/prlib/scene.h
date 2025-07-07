#ifndef PRLIB_SCENE_H
#define PRLIB_SCENE_H

#include "common.h"

#include <eetypes.h>
#include <eestruct.h>

class PrSceneObject {
public:
    void PreprocessModel();

public:
    PR_PADDING(unk0, 0x50);
    sceGsFrame unk50;
    sceGsXyoffset unk58;
};

#endif /* PRLIB_SCENE_H */
