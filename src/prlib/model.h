#ifndef PRLIB_MODEL_H
#define PRLIB_MODEL_H

#include "nalib/navector.h"
#include <eetypes.h>

struct SpmFileHeader;

struct SpmNode {
public:
    void ChangePointer(SpmFileHeader* arg0, SpmNode* arg1);
};

struct SpmFileHeader {
public:
    char unk0[0x6];
    u_short unk6;
    char unk8[0x5c];
    int unk64;
    u_int unk68;
    char unk6C[0x8];
    SpmNode** unk74;

public:
    void ChangePointer();
};

class PrModelObject {
public:
    void GetPrimitivePosition(NaVECTOR<float, 4>* arg0);
    void GetScreenPosition(NaVECTOR<float, 4>* arg0);
};

#endif