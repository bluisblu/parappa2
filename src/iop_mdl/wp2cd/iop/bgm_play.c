#include "common.h"

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", Wp2CdStRead);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", readPC);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", usrMemcpy);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", TransBufSet);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", IntFunc);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", IntFuncEnd);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", makeMyThread);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", makeMyThread_Tr);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", makeMySem);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetVolumeDirect);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetMasterVolume);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmCdInit);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSdInit);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmInit);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmQuit);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmOpen);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmOpenFLoc);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmClose);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", TransBufSet_SUB);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmPreLoad);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _PreLoadBack);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmPreLoadBack);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmReadBuffFull);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmStart);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _BgmStop);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmStop);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetVolume);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetMode);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmGetMode);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", GetKoubaisuu);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSeek);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSeekFLoc);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetChannel);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetTrPoint);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmGetTime);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmGetTSample);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmGetCdErrCode);

IOP_INCLUDE_RODATA("wp2cd/nonmatchings/iop/bgm_play", D_00003A50);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _BgmPlay);
