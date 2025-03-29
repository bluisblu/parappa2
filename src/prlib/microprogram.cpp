#include "common.h"

INCLUDE_ASM("prlib/microprogram", CalculateEntryPoint__Fv);

INCLUDE_ASM("prlib/microprogram", PrLoadMicroPrograms__Fv);

INCLUDE_ASM("prlib/microprogram", PrGetMicroProgramAddress__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetInputVertexParameterNum__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetInputVertexParameterSet__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetOutputVertexParameterNum__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetOutputVertexParameterSet__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetMendererCreateTextureAddress__Fv);

INCLUDE_ASM("prlib/microprogram", PrGetMendererDrawMeshAddress__Fv);
