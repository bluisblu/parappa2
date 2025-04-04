#ifndef PRLIB_SPADATA_H
#define PRLIB_SPADATA_H

#include "common.h"

#include <eetypes.h>

struct SpaTransform {
public:
    int IsEverIdentical();
};
    
struct SpaNodeAnimation {
public:
    char unk0[0x8];
    u_int unk8;
    SpaTransform** unkC;
    
public:
    int Optimize();
};

#endif