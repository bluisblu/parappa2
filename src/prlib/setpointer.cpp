#include "common.h"

#include "prlib/model.h"

/* TODO: Cleaner way of doing this? */
#define GetPointer(type, x)  \
    (x == NULL) ? NULL :     \
    reinterpret_cast<type>(reinterpret_cast<int>(this) + reinterpret_cast<int>(x))

void SpmFileHeader::ChangePointer() {
    if (this->unk6 & 0x1) {
        return;
    }

    this->unk64 = GetPointer(int, this->unk64);
    this->unk74 = GetPointer(SpmNode**, this->unk74);

    for (int i = 0; i < this->unk68; i++) {
        this->unk74[i] = GetPointer(SpmNode*, this->unk74[i]);
    }

    (*this->unk74)->ChangePointer(this, NULL);
    this->unk6 |= 0x1;
}

INCLUDE_ASM("prlib/setpointer", ChangePointer__7SpmNodeP13SpmFileHeaderP7SpmNode);

INCLUDE_ASM("prlib/setpointer", ChangePointer__13SpaFileHeader);

INCLUDE_ASM("prlib/setpointer", ChangePointer__16SpaNodeAnimationP13SpaFileHeader);

INCLUDE_ASM("prlib/setpointer", ChangePointer__13SpcFileHeader);
