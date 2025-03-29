#include "common.h"

INCLUDE_ASM("prlib/wave", CG_WaveInit__FP8WAVE_STR);

INCLUDE_ASM("prlib/wave", CG_WaveInitEasy__FP8WAVE_STRssss10WMODE_ENUM);

INCLUDE_ASM("prlib/wave", UG_WaveDisp__FP8WAVE_STRP10sceGsFrameP12sceGifPacket);

INCLUDE_ASM("prlib/wave", CG_WaveDisp__FP8WAVE_STRP10sceGsFrame);

INCLUDE_ASM("prlib/wave", WaveCtrlInit__FP8WAVE_STRss10WMODE_ENUM);

INCLUDE_ASM("prlib/wave", WaveCtrlDisp__FP8WAVE_STRP10sceGsFrame);

INCLUDE_ASM("prlib/wave", WaveCtrlUpdate__FP8WAVE_STRf);

INCLUDE_RODATA("prlib/wave", D_003969A0);
