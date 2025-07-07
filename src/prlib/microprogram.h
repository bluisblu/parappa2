#ifndef PRLIB_MICROPROGRAM_H
#define PRLIB_MICROPROGRAM_H

#include "common.h"

#include <eetypes.h>

enum PrMICRO_PROGRAM_MODULE {
    eVumpNormal,
    eVumpBothface,
    eVumpContour,
    eVumpRefmap,
    eVumpScreen,
    eVumpAntiline,
};

void PrLoadMicroPrograms();
u_int PrGetMicroProgramAddress(PrMICRO_PROGRAM_MODULE module);

u_int PrGetInputVertexParameterNum(PrMICRO_PROGRAM_MODULE module);
u_int PrGetInputVertexParameterSet(PrMICRO_PROGRAM_MODULE module);

u_int PrGetOutputVertexParameterNum(PrMICRO_PROGRAM_MODULE module);
u_int PrGetOutputVertexParameterSet(PrMICRO_PROGRAM_MODULE module);

u_int PrGetMendererCreateTextureAddress();
u_int PrGetMendererDrawMeshAddress();

#endif /* PRLIB_MICROPROGRAM_H */
