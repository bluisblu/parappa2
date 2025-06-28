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

template <>
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

bool SpaTransform::IsEverIdentical() {
    switch (this->unk0) {
    case 0:
        if (this->unk14 == 1) {
            bool v0 = false;
            if (this->unk20[0][0] == 1.0f) {
                if (this->unk20[0][1] != 1.0f) {
                    return false;
                }
                if (this->unk20[0][2] == 1.0f) {
                    v0 = true;
                }
            }
            return v0;
        }

        return false;

    case 1:
        if (this->unk14 == 1) {
            if (this->unk20[0][3] == 0.0f) {
                return true;
            }
        }

        return false;

    case 2:
    case 3:
    case 4:
        if (this->unk14 == 1) {
            if (this->unk20[0][0] == 0.0f) {
                return true;
            }
        }

        return false;

    case 5:
        if (this->unk14 == 1) {
            bool v0 = false;
            if (this->unk20[0][0] == 0.0f) {
                if (this->unk20[0][1] != 0.0f) {
                    return false;
                }
                if (this->unk20[0][2] == 0.0f) {
                    v0 = true;
                }
            }
            return v0;
        }

        return false;

    case 6:
        if (this->unk14 == 1) {
            return this->unk20.inl0();
        }

        return false;
        
    case 7:
        if (this->unk14 == 1) {
            bool v0 = false;
            if (this->unk20[0][0] == 0.0f) {
                if (this->unk20[0][1] != 0.0f) {
                    return false;
                }
                if (this->unk20[0][2] == 0.0f) {
                    v0 = true;
                }
            }
            return v0;
        }

        return false;

    default:
        NaMATRIX<float, 4, 4>& a2 = NaMATRIX<float, 4, 4>::IDENT;
        return a2.inl1();
    }
}

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
