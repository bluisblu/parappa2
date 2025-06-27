#include "prlib/spadata.h"

#include <math.h>

u_int SpaTrackBase::SearchSegment(float arg0) const {
    if (this->unk4 == 1) {
        return (u_int)-1;
    }

    if (arg0 <= this->unkC[0]) {
        return (u_int)-1;
    }

    if (arg0 >= this->unkC[this->unk4 - 1]) {
        return this->unk4;
    }

    if (arg0 >= this->unkC[this->unk8]) {
        if (arg0 < this->unkC[this->unk8 + 1]) {
            return this->unk8;
        }

        if ((this->unk8 + 2) < this->unk4) {
            if (arg0 < this->unkC[this->unk8 + 2]) {
                return ++this->unk8;
            }
        }
    } else if (this->unk8 != 0) {
        if (arg0 >= this->unkC[this->unk8 - 1]) {
            return --this->unk8;
        }
    }

    int right = this->unk4 - 2;
    int left = 0;
    int mid = 0;

    while (left <= right) {
        mid = (left + right) / 2;
        if (arg0 < this->unkC[mid]) {
            right = mid - 1;
        } else if (arg0 >= this->unkC[mid + 1]) {
            left = mid + 1;
        } else {
            break;
        }
    }

    this->unk8 = mid;
    return this->unk8;
}

int* SpaTrack<int>::GetValue(float arg0) const {
    if (this->unk2 & 0x1) {
        float f13 = this->unkC[this->unk4 - 1];
        if (arg0 < 0.0f || arg0 >= f13) {
            arg0 = fmodf(arg0, f13);
        }
    }

    u_int seg = this->SearchSegment(arg0);
    if (seg == (u_int)-1) {
        return (int*)&this->unk10;
    }

    if (seg == this->unk4) {
        return (int*)&this->unkC + seg;
    } else {
        return (int*)&this->unk10 + seg;
    }
}

INCLUDE_ASM("prlib/spadata", GetMatrix__C12SpaTransformf);

INCLUDE_ASM("prlib/spadata", GetMatrix__C16SpaNodeAnimationf);

bool SpaNodeAnimation::IsVisible(float arg0) const {
    if (this->unk4 == NULL) {
        return true;
    }

    return *this->unk4->GetValue(arg0);
}

INCLUDE_ASM("prlib/spadata", IsNodeVisible__C13SpaFileHeaderP7SpmNodef);

INCLUDE_ASM("prlib/spadata", IsEverIdentical__12SpaTransform);

int SpaNodeAnimation::Optimize() {
    int remove_count = 0;

    for (int i = 0; i < this->unk8; i++) {
        SpaTransform* transform = this->unkC[i];
        if (transform == NULL || transform->IsEverIdentical()) {
            remove_count++;
            continue;
        }
        this->unkC[i - remove_count] = this->unkC[i];
    }

    this->unk8 -= remove_count;
    return remove_count;
}

INCLUDE_ASM("prlib/spadata", GetLinearValue__Ct8SpaTrack1Zt8NaVECTOR2Zfi4Uif);

INCLUDE_ASM("prlib/spadata", GetValue__Ct8SpaTrack1Zt8NaVECTOR2Zfi4f);

/* nalib/navector.h */
INCLUDE_ASM("prlib/spadata", func_00149168);

INCLUDE_ASM("prlib/spadata", func_001491C0);

INCLUDE_ASM("prlib/spadata", func_00149270);

/* prlib/spadata.cpp */
INCLUDE_ASM("prlib/spadata", GetSprineValue__Ct8SpaTrack1ZfUif);

INCLUDE_ASM("prlib/spadata", GetLinearValue__Ct8SpaTrack1ZfUif);

INCLUDE_ASM("prlib/spadata", GetValue__Ct8SpaTrack1Zff);

INCLUDE_ASM("prlib/spadata", GetSprineValue__Ct8SpaTrack1Zt8NaMATRIX3Zfi4i4Uif);

INCLUDE_ASM("prlib/spadata", GetLinearValue__Ct8SpaTrack1Zt8NaMATRIX3Zfi4i4Uif);

INCLUDE_ASM("prlib/spadata", GetValue__Ct8SpaTrack1Zt8NaMATRIX3Zfi4i4f);

/* nalib/navector.h */
INCLUDE_ASM("prlib/spadata", func_0014ABE0);

INCLUDE_ASM("prlib/spadata", func_0014ACE8);

INCLUDE_ASM("prlib/spadata", func_0014AFE0);

/* prlib/spadata.cpp */
INCLUDE_ASM("prlib/spadata", GetSprineValue__Ct8SpaTrack1Zt8NaVECTOR2Zfi4Uif);
