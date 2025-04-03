#include "common.h"

#include <eetypes.h>
#include <eestruct.h>

#include "prlib/model.h"

void SpmFileHeader::ChangePointer() {
    if (this->unk6 & 0x1) {
        return;
    }

    this->unk64 = (int)CalculatePointer((void*)this->unk64);
    this->unk74 = (SpmNode**)CalculatePointer((void*)this->unk74);

    for (int i = 0; i < this->unk68; i++) {
        this->unk74[i] = (SpmNode*)CalculatePointer((void*)this->unk74[i]);
    }

    /* Change node pointers starting from the first. */
    (*this->unk74)->ChangePointer(this, NULL);
    this->unk6 |= 0x1;
}

void SpmNode::ChangePointer(SpmFileHeader* arg0, SpmNode* arg1) {
    this->unk158 = arg0;
    this->unk164 = arg1;

    this->unk15C = (SpmNode*)arg0->CalculatePointer((void*)this->unk15C);
    this->unk160 = (SpmNode*)arg0->CalculatePointer((void*)this->unk160);

    this->unk16C[0] = (SpmNodeUnk*)arg0->CalculatePointer((void*)this->unk16C[0]);
    this->unk16C[1] = (SpmNodeUnk*)arg0->CalculatePointer((void*)this->unk16C[1]);

    this->unk17C = (int)this->unk16C[0];
    if (this->unk16C[0] == 0) {
        this->unk17C = (int)this->unk16C[1];
    }

    for (u_int i = 0; i < 2; i++) {
        if (this->unk16C[i]) {
            this->unk16C[i]->unk194 = SCE_VIF1_SET_MSCAL(PrGetMicroProgramAddress(this->unk16C[i]->unk60), 0);
        }
    }

    if (this->unk154 & 0xff0) {
        this->unk198 = (int)arg0->CalculatePointer((void*)this->unk198);
        if (this->unk154 & 0x10) {
            this->unk1B4 = (int)arg0->CalculatePointer((void*)this->unk1B4);
            this->unk1B8 = (int)arg0->CalculatePointer((void*)this->unk1B8);
        } else if (this->unk154 & 0x20) {
            this->unk1B4 = (int)arg0->CalculatePointer((void*)this->unk1B4);
        }

        this->unk1A0 = (int)arg0->CalculatePointer((void*)this->unk1A0);
        this->unk1A4 = (SpmNodeUnk*)arg0->CalculatePointer((void*)this->unk1A4);
        SpmNodeUnk* s0 = this->unk1A4;
        if (s0 != NULL) {
            s0->unk194 = SCE_VIF1_SET_MSCAL(PrGetMicroProgramAddress(ePrMicroProgram_Unk2), 0);
        }
    }

    SpmNode* s0 = this->unk15C;
    while (s0 != 0) {
        s0->ChangePointer(arg0, this);
        s0 = s0->unk160;
    }
}

INCLUDE_ASM("prlib/setpointer", ChangePointer__13SpaFileHeader);

INCLUDE_ASM("prlib/setpointer", ChangePointer__16SpaNodeAnimationP13SpaFileHeader);

INCLUDE_ASM("prlib/setpointer", ChangePointer__13SpcFileHeader);
