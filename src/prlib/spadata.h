#ifndef PRLIB_SPADATA_H
#define PRLIB_SPADATA_H

#include "common.h"

#include <eetypes.h>

#include <nalib/namatrix.h>

class SpaTrackBase {
public:
    u_int SearchSegment(float arg0) const;

protected:
    PR_PADDING(unk0, 0x2);
    u_short unk2;
    u_int unk4;
    mutable u_int unk8;
    float* unkC;
};

template <typename T>
class SpaTrack : public SpaTrackBase {
public:
    T* GetValue(float arg0) const;

private:
    T* unk10;
};

class SpaTransform {
public:
    bool IsEverIdentical();

private:
    u_char unk0;
    PR_PADDING(unk1, 0x13);
    int unk14;
    PR_PADDING(unk18, 0x8);
    NaMATRIX<float, 4, 4> unk20;
};
    
class SpaNodeAnimation {   
public:
    bool IsVisible(float arg0) const;
    int Optimize();

public:
    PR_PADDING(unk0, 0x4);
    SpaTrack<int>* unk4;
    u_int unk8;
    SpaTransform** unkC;
};

#endif /* PRLIB_SPADATA_H */
