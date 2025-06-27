#ifndef PRLIB_SPADATA_H
#define PRLIB_SPADATA_H

#include "common.h"

#include <eetypes.h>

class SpaTrackBase {
public:
    u_int SearchSegment(float arg0) const;

protected:
    char unk0[0x2];
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
    int IsEverIdentical();
};
    
class SpaNodeAnimation {   
public:
    bool IsVisible(float arg0) const;
    int Optimize();

private:
    char unk0[0x4];
    SpaTrack<int>* unk4;
    u_int unk8;
    SpaTransform** unkC;
};

#endif