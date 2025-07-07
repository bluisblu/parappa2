#ifndef PRLIB_MODEL_H
#define PRLIB_MODEL_H

#include "common.h"

#include <eetypes.h>

#include "prlib/microprogram.h"

#include <nalib/navector.h>

struct SpmFileHeader;

struct SpmNodeUnk {
    PR_PADDING(unk0, 0x60);
    PrMICRO_PROGRAM_MODULE unk60;
    PR_PADDING(unk64, 0x130);
    int unk194;
};

struct SpmNode {
public:
    PR_PADDING(unk0, 0x150);
    int unk150;
    int unk154;
    SpmFileHeader* unk158;
    SpmNode* unk15C;
    SpmNode* unk160;
    SpmNode* unk164;
    PR_PADDING(unk168, 0x4);
    SpmNodeUnk* unk16C[2];
    PR_PADDING(unk174, 0x8);
    int unk17C;
    PR_PADDING(unk180, 0x17);
    int unk198;
    PR_PADDING(unk19C, 0x4);
    int unk1A0;
    SpmNodeUnk* unk1A4;
    PR_PADDING(unk1A8, 0xc);
    int unk1B4;
    int unk1B8;

public:
    void ChangePointer(SpmFileHeader* arg0, SpmNode* arg1);
};

struct SpmFileHeader {
public:
    PR_PADDING(unk0, 0x6);
    u_short unk6;
    PR_PADDING(unk8, 0x5c);
    int unk64;
    u_int unk68;
    PR_PADDING(unk6C, 0x8);
    SpmNode** unk74;

public:
    void ChangePointer();

    void* CalculatePointer(void* offset) {
        if (!offset) {
            return NULL;
        }
        return reinterpret_cast<void*>(reinterpret_cast<int>(this) + reinterpret_cast<int>(offset));
    }
};

class PrModelObject {
public:
    void GetPrimitivePosition(NaVECTOR<float, 4>* arg0);
    void GetScreenPosition(NaVECTOR<float, 4>* arg0);
};

#endif /* PRLIB_MODEL_H */
