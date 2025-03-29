#ifndef PRLIB_SCENE_H
#define PRLIB_SCENE_H

#include <eetypes.h>
#include <eestruct.h>

class PrSceneObject {
public:
    void PreprocessModel();
public:
    char unk0[0x50];
    sceGsFrame unk50;
    sceGsXyoffset unk58;
};

#endif