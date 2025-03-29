#ifndef PRLIB_MODEL_H
#define PRLIB_MODEL_H

#include "nalib/navector.h"

class PrModelObject {
public:
    void GetPrimitivePosition(NaVECTOR<float, 4>* arg0);
    void GetScreenPosition(NaVECTOR<float, 4>* arg0);
};

#endif