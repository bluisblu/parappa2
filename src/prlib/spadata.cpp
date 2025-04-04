#include "prlib/spadata.h"

INCLUDE_ASM("prlib/spadata", SearchSegment__C12SpaTrackBasef);

INCLUDE_ASM("prlib/spadata", GetValue__Ct8SpaTrack1Zif);

INCLUDE_ASM("prlib/spadata", GetMatrix__C12SpaTransformf);

INCLUDE_ASM("prlib/spadata", GetMatrix__C16SpaNodeAnimationf);

INCLUDE_ASM("prlib/spadata", IsVisible__C16SpaNodeAnimationf);

INCLUDE_ASM("prlib/spadata", IsNodeVisible__C13SpaFileHeaderP7SpmNodef);

INCLUDE_ASM("prlib/spadata", IsEverIdentical__12SpaTransform);

int SpaNodeAnimation::Optimize() {
    int removeCount = 0;

    for (int i = 0; i < this->unk8; i++) {
        SpaTransform* transform = this->unkC[i];
        if (transform == NULL || transform->IsEverIdentical()) {
            removeCount++;
            continue;
        }
        this->unkC[i - removeCount] = this->unkC[i];
    }

    this->unk8 -= removeCount;
    return removeCount;
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
