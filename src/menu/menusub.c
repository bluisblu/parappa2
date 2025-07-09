#include "menu/menusub.h"

#include "os/syssub.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

/* sdata 3997bc */ extern TSTEX_INF *tblTex; /* static */
/* sdata 399824 */ extern HOSI_OBJ *HOSIObj; /* static */
/* bss 1c77ac0 */ extern TSREPPAD menuPadState[2][4]; /* static */
/* bss 1c77c10 */ extern BGMSTATE TsBGMState; /* static */
/* data 18cc38 */ extern MN_SCENE MNS_StageMap;
/* bss 1c77cb0 */ extern MN_SCENE MNS_StageMap2; /* static */
/* bss 1c78e08 */ extern MN_SCENE MNS_CityHall; /* static */
/* bss 1c79f60 */ extern MN_SCENE MNS_OptCounter; /* static */
/* bss 1c7b0b8 */ extern MN_SCENE MNS_RepCounter; /* static */
/* bss 1c7c210 */ extern MN_SCENE MNS_StgCounter[2]; /* static */
/* bss 1c7e4b8 */ extern MN_SCENE MNS_JimakuBak; /* static */
/* sbss 399b34 */ extern USER_DATA *UserWork; /* static */
/* sbss 399b38 */ extern P3MC_STAGERANK *pCStageRank; /* static */
/* sbss 399b3c */ extern P3MC_USRLST *UserLst; /* static */
/* sbss 399b40 */ extern MN_USERLST_WORK *UserDispWork; /* static */
/* bss 1c7f628 */ extern CURFILEINFO CurFileInfo; /* static */

static int TsGetMenuPadIsRepeat(int no, int npad) {
    return (menuPadState[no][npad].state < 2) ^ 1;
}

INCLUDE_ASM("menu/menusub", TSSNDPLAY);

INCLUDE_ASM("menu/menusub", TSSNDSTOP);

INCLUDE_ASM("menu/menusub", TSSNDMASK_CHAN);

INCLUDE_ASM("menu/menusub", TSSND_SKIPSTOP);

INCLUDE_ASM("menu/menusub", TSSND_SKIPPLAY);

INCLUDE_ASM("menu/menusub", TSSND_CHANISSTOP);

INCLUDE_ASM("menu/menusub", tsBGMONEPlay);

INCLUDE_ASM("menu/menusub", tsBGMONEStop);

INCLUDE_ASM("menu/menusub", tsBGMONEVol);

INCLUDE_ASM("menu/menusub", tsBGMONETop);

INCLUDE_ASM("menu/menusub", tsBGMONEflow);

INCLUDE_ASM("menu/menusub", tsBGMONEPause);

void TsBGMInit(void) {
    memset(&TsBGMState, 0, sizeof(TsBGMState));
}

INCLUDE_ASM("menu/menusub", TsBGMPlay);

INCLUDE_ASM("menu/menusub", TsBGMStop);

INCLUDE_ASM("menu/menusub", TsBGMMute);

INCLUDE_ASM("menu/menusub", TsBGMLoadCheck);

INCLUDE_ASM("menu/menusub", TsBGMPause);

INCLUDE_ASM("menu/menusub", TsBGMChangePos);

INCLUDE_ASM("menu/menusub", TsBGMPoll);

INCLUDE_ASM("menu/menusub", TsCmnPkOpen);

INCLUDE_ASM("menu/menusub", TsCmnPkClose);

INCLUDE_ASM("menu/menusub", _P3DATA_SIZE);

INCLUDE_ASM("menu/menusub", TsGetTm2Tex);

INCLUDE_ASM("menu/menusub", TsGetTm2HedTex);

INCLUDE_ASM("menu/menusub", TsClearMenuPad);

INCLUDE_ASM("menu/menusub", TsGetMenuPad);

INCLUDE_ASM("menu/menusub", TsSndFlow);

INCLUDE_ASM("menu/menusub", TSNumMov);

INCLUDE_ASM("menu/menusub", TSNumRBack);

static int TSLOOP(int no, int max) {
    return (no + max) % max;
}

static int TSLIMIT(int no, int min, int max) {
    if (no < min) {
        return min;
    }
    if (no >= max) {
        return max - 1;
    }
    return no;
}

INCLUDE_ASM("menu/menusub", TsMENU_GetMapNo);

INCLUDE_ASM("menu/menusub", TsMENU_GetMapTimeState);

INCLUDE_ASM("menu/menusub", TsSetScene_Map);

INCLUDE_ASM("menu/menusub", TsSet_ParappaCapColor);

INCLUDE_RODATA("menu/menusub", D_00395F10);

INCLUDE_RODATA("menu/menusub", D_00395F30);

INCLUDE_ASM("menu/menusub", TsClearSet);

INCLUDE_ASM("menu/menusub", TsCheckEnding);

void TsMENU_InitSystem(void) {
    int i;

    P3MC_InitReady();
    P3MC_CheckChangeSet();

    memset(&MNS_JimakuBak, 0, sizeof(MNS_JimakuBak));
    memset(&MNS_RepCounter, 0, sizeof(MNS_RepCounter));
    memset(&MNS_OptCounter, 0, sizeof(MNS_OptCounter));
    memset(&MNS_CityHall, 0, sizeof(MNS_CityHall));
    memset(&MNS_StageMap, 0, sizeof(MNS_StageMap));
    memset(&MNS_StageMap2, 0, sizeof(MNS_StageMap2));

    for (i = 0; i < 2; i++) {
        memset(&MNS_StgCounter[i], 0, sizeof(MNS_StgCounter[i]));
    }

    tblTex = NULL;
    UserLst = NULL;
    UserDispWork = NULL;

    UserWork = (USER_DATA*)memalign(16, sizeof(USER_DATA));
    memset(UserWork, 0, sizeof(USER_DATA));

    pCStageRank = (P3MC_STAGERANK*)memalign(16, sizeof(P3MC_STAGERANK[8]));
    memset(pCStageRank, 0, sizeof(P3MC_STAGERANK[8]));

    memset(&CurFileInfo, 0, sizeof(CURFILEINFO));
    CurFileInfo.logFileNo = -1;
    CurFileInfo.repFileNo = -1;
}

INCLUDE_ASM("menu/menusub", TsMENU_EndSystem);

INCLUDE_ASM("menu/menusub", TsMenu_RankingClear);

INCLUDE_ASM("menu/menusub", TsMenu_Init);

INCLUDE_ASM("menu/menusub", TsMenu_End);

INCLUDE_ASM("menu/menusub", TsMenu_InitFlow);

INCLUDE_ASM("menu/menusub", TsMenuMemcChk_Flow);

INCLUDE_ASM("menu/menusub", TsMenu_Flow);

INCLUDE_ASM("menu/menusub", TsMenu_Draw);

INCLUDE_ASM("menu/menusub", TsSetRankingName);

INCLUDE_ASM("menu/menusub", TsSetRanking2UData);

INCLUDE_ASM("menu/menusub", TsSetSaveData);

INCLUDE_ASM("menu/menusub", TsRestoreSaveData);

int DateChgInt(u_int n) {
    /* Convert BCD to decimal */
    return 
    (
        ((n & 0xf0) >> 4) * 10 +
         (n & 0xf)
    );
}

INCLUDE_ASM("menu/menusub", GetRankScoreID);

INCLUDE_ASM("menu/menusub", TsRanking_Set);

INCLUDE_ASM("menu/menusub", TsMENU_SetMapScreen);

INCLUDE_ASM("menu/menusub", TsCheckTimeMapChange);

int TsAnimeWait_withKeySkip(u_int tpad, MN_SCENE *scene, int ltim, u_int bnk) {
    if (bnk == -1) {
        return MNScene_isAnime(scene, ltim);
    } else {
        return MNScene_isAnimeBank(scene, ltim, bnk);
    }
}

INCLUDE_ASM("menu/menusub", TsMemCardCheck_Flow);

INCLUDE_ASM("menu/menusub", TsMap_Flow);

INCLUDE_ASM("menu/menusub", TsMakeUserWork);

INCLUDE_ASM("menu/menusub", TsSaveSuccessProc);

INCLUDE_ASM("menu/menusub", MpSave_Flow);

INCLUDE_ASM("menu/menusub", MpCityHall_Flow);

INCLUDE_ASM("menu/menusub", MpCityHallParaStart);

INCLUDE_ASM("menu/menusub", MpCityHallFPHSSoundMask);

INCLUDE_ASM("menu/menusub", MpCityHallFPHSMove);

INCLUDE_ASM("menu/menusub", MpCityHallFPHOK);

INCLUDE_ASM("menu/menusub", MpCityHallCharPosSet);

INCLUDE_ASM("menu/menusub", MpPopMenu_Flow);

INCLUDE_ASM("menu/menusub", MpMapMenu_Flow);

INCLUDE_ASM("menu/menusub", _MapGetMovableDir);

INCLUDE_ASM("menu/menusub", McErrorMess);

INCLUDE_ASM("menu/menusub", McInitFlow);

INCLUDE_ASM("menu/menusub", McStartCheckFlow);

INCLUDE_ASM("menu/menusub", McUserCheckFlow);

INCLUDE_ASM("menu/menusub", McUserSaveFlow);

INCLUDE_ASM("menu/menusub", McUserLoadFlow);

INCLUDE_ASM("menu/menusub", TsMCAMes_Init);

INCLUDE_ASM("menu/menusub", TsMCAMes_GetSelect);

INCLUDE_ASM("menu/menusub", TsMCAMes_IsON);

INCLUDE_ASM("menu/menusub", TsMCAMes_SetPos);

INCLUDE_ASM("menu/menusub", TsMCAMes_SetMes);

INCLUDE_ASM("menu/menusub", TsMCAMes_Flow);

INCLUDE_ASM("menu/menusub", TsMCAMes_Draw);

INCLUDE_ASM("menu/menusub", TsCMPMes_SetPos);

INCLUDE_ASM("menu/menusub", TsCMPMes_SetMes);

INCLUDE_ASM("menu/menusub", TsCMPMes_Draw);

INCLUDE_ASM("menu/menusub", TsANIME_Init);

INCLUDE_ASM("menu/menusub", TsANIME_Poll);

INCLUDE_ASM("menu/menusub", TsANIME_Start);

INCLUDE_ASM("menu/menusub", TsANIME_GetRate);

INCLUDE_ASM("menu/menusub", _TsSortSetRanking);

INCLUDE_ASM("menu/menusub", TsGetRankingList);

INCLUDE_ASM("menu/menusub", TsPopCusAOff);

INCLUDE_ASM("menu/menusub", TsPopCusDim);

INCLUDE_ASM("menu/menusub", TsPopCusInit);

INCLUDE_ASM("menu/menusub", TsPopCusFlow);

INCLUDE_ASM("menu/menusub", TsPopCusPut);

INCLUDE_ASM("menu/menusub", TsPUPCheckMove);

INCLUDE_RODATA("menu/menusub", D_00396068);

INCLUDE_RODATA("menu/menusub", D_00396070);

INCLUDE_RODATA("menu/menusub", D_00396078);

INCLUDE_ASM("menu/menusub", TsPopMenu_Flow);

INCLUDE_ASM("menu/menusub", TsPopMenu_Draw);

INCLUDE_ASM("menu/menusub", TsPopMenCus_Draw);

INCLUDE_ASM("menu/menusub", TsSaveMenu_Flow);

INCLUDE_ASM("menu/menusub", TsSaveMenu_Draw);

INCLUDE_ASM("menu/menusub", TSJukeCDObj_Init);

INCLUDE_ASM("menu/menusub", _TsJkJacketPut);

INCLUDE_ASM("menu/menusub", _TsJkRecordPut);

INCLUDE_ASM("menu/menusub", TSJukeCDObj_Draw);

INCLUDE_ASM("menu/menusub", TsJukeIsObjAnime);

INCLUDE_ASM("menu/menusub", TsJukeObjAnime);

INCLUDE_ASM("menu/menusub", TsJukeObjAnime2);

INCLUDE_ASM("menu/menusub", _TsJKMoveCus);

INCLUDE_ASM("menu/menusub", _TsJKSetPadArrow);

INCLUDE_ASM("menu/menusub", TsJukeMenu_Flow);

INCLUDE_ASM("menu/menusub", TsJukeMenu_Draw);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorSET);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorON);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorOFF);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorSEL);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorMASK);

INCLUDE_ASM("menu/menusub", TsCmnCell_CusorDraw);

INCLUDE_ASM("menu/menusub", TsOption_Flow);

INCLUDE_ASM("menu/menusub", TsOption_Draw);

INCLUDE_ASM("menu/menusub", TsUserList_GetCurFileNo);

INCLUDE_ASM("menu/menusub", TsUserList_IsGetFileSave);

INCLUDE_ASM("menu/menusub", TsUserList_SortUser);

INCLUDE_ASM("menu/menusub", TsUserList_SetCurUserData);

INCLUDE_ASM("menu/menusub", TsUserList_SetCurDispUserData);

INCLUDE_ASM("menu/menusub", TsUserList_SetCurFileNoCusor);

INCLUDE_ASM("menu/menusub", TsUserList_SetType);

INCLUDE_ASM("menu/menusub", TsUserList_TagChangeAble);

INCLUDE_ASM("menu/menusub", TsUserList_SetCurTag);

INCLUDE_ASM("menu/menusub", TsUserList_Flow);

INCLUDE_ASM("menu/menusub", TsUserList_Draw);

INCLUDE_ASM("menu/menusub", NameSpaceCut);

INCLUDE_RODATA("menu/menusub", D_00396120);

INCLUDE_ASM("menu/menusub", TsUser_PanelDraw);

INCLUDE_ASM("menu/menusub", TsNAMEINBox_SetName);

INCLUDE_ASM("menu/menusub", TsNAMEINBox_GetName);

INCLUDE_ASM("menu/menusub", TsNAMEINBox_Flow);

INCLUDE_ASM("menu/menusub", TsNAMEINBox_Draw);

INCLUDE_ASM("menu/menusub", TsSCFADE_Set);

INCLUDE_ASM("menu/menusub", TsSCFADE_Flow);

INCLUDE_ASM("menu/menusub", TsSCFADE_Draw);

INCLUDE_ASM("menu/menusub", _PkMCMsgPut);

INCLUDE_ASM("menu/menusub", _PkMCMsgGetLine);

INCLUDE_ASM("menu/menusub", _PkSubMsgPut);

INCLUDE_ASM("menu/menusub", TsMenu_CleanVram);

void TsMenu_CaptureVram(SPR_PKT pk, SPR_PRM *spr) {
    PkSprPkt_SetDrawEnv(pk, spr, DrawGetDrawEnvP(DNUM_VRAM2));
    PkSprPkt_SetTexVram(pk, spr, DrawGetDrawEnvP(DNUM_DRAW));

    PkTEX1_Add(pk, SCE_GS_SET_TEX1(0, 0, 0, 0, 0, 0, 0));
    PkALPHA_Add(pk, SCE_GS_SET_ALPHA(2, 2, 0, 0, 0));

    spr->zy = 1.0f;
    spr->zx = 1.0f;
    spr->zdepth = 0;
    spr->rgba0 = SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0);

    SetSprScreenXYWH(spr);
    spr->ux = spr->px;
    spr->uy = spr->py;
    spr->uw = spr->sw;
    spr->uh = spr->sh;

    PkNSprite_AddAdj(pk, spr, 1);
    PkTEX1_Add(pk, 0x2020);

    PkSprPkt_SetDrawEnv(pk, spr, DrawGetDrawEnvP(DNUM_DRAW));
    PkSprPkt_SetTexVram(pk, spr, DrawGetDrawEnvP(DNUM_VRAM2));

    PkALPHA_Add(pk, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));
}

INCLUDE_ASM("menu/menusub", TsSetCTransSpr);

INCLUDE_ASM("menu/menusub", TsSetSLTransSpr);

INCLUDE_ASM("menu/menusub", TsSetPNTransSpr);
void TsSetPNTransSpr(/* s3 19 */ SPR_PKT pk, /* s2 18 */ SPR_PRM *spr, /* a2 6 */ int mx, /* a3 7 */ int my, /* f12 50 */ float wr, /* f22 60 */ float dr);

INCLUDE_ASM("menu/menusub", TsPatTexFnc);

INCLUDE_ASM("menu/menusub", _TsPatSetPrm);
/* static */ void _TsPatSetPrm(/* t1 9 */ SPR_PKT pk, /* s0 16 */ SPR_PRM *spr, /* s2 18 */ PATPOS *ppos, /* s3 19 */ int ox, /* s4 20 */ int oy);

INCLUDE_ASM("menu/menusub", TsPatPut);

static void TsPatGetSize(PATPOS *ppos, int *x, int *y, int *w, int *h) {
    TSTEX_INF *ptex = &tblTex[ppos->texNo];

    if (w != NULL) {
        *w = ptex->w;
    }
    if (h != NULL) {
        *h = ptex->h;
    }
    if (x != NULL) {
        *x = ppos->x + (ptex->w / 2);
    }
    if (y != NULL) {
        *y = ppos->y + (ptex->h / 2);
    }
}

INCLUDE_ASM("menu/menusub", TsPatPutRZoom);

INCLUDE_ASM("menu/menusub", TsPatPutMZoom);

INCLUDE_ASM("menu/menusub", TsPatPutSwing);

static void TsPatPutUneri(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy, int mx, int my, float Crx, float Drt) {
    _TsPatSetPrm(pk, spr, ppos, ox, oy);
    TsSetPNTransSpr(pk, spr, mx, my, Crx, Drt);
}

INCLUDE_ASM("menu/menusub", TsCELBackInit);

void TsCELBackEnd(void) {
    if (HOSIObj != NULL) {
        free(HOSIObj);
        HOSIObj = NULL;
    }
}

INCLUDE_ASM("menu/menusub", TsCELBackDraw);

INCLUDE_ASM("menu/menusub", _TsCELBackObjDraw);

INCLUDE_ASM("menu/menusub", TsHosiPut);

INCLUDE_RODATA("menu/menusub", D_00396168);

INCLUDE_RODATA("menu/menusub", D_00396170);
