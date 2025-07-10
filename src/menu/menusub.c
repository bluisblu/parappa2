#include "menu/menusub.h"

#include "main/cdctrl.h"
#include "menu/menufont.h"
#include "menu/p3mc.h"
#include "os/syssub.h"
#include "os/tim2.h"

#include <prlib/prlib.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

// /* data 18b0e0 */ static MNMAPPOS mnmapMap1[0];
// /* data 18b160 */ static MNMAPPOS mnmapMap[0];
// /* data 18b3a0 */ static MNMAPPOS mnmapMap2[0];
// /* data 18b620 */ static short RShopRute0[0];
// /* data 18b630 */ static short RShopRute1[0];
// /* data 18b638 */ static short RShopRute2[0];
// /* data 18b648 */ static short *RecordShopRute[10];
// /* data 18b670 */ static MNMAPPOS mnmapCityHall[0];
// /* data 18b730 */ short AnmCHallPara_OptRet[0];
// /* data 18b738 */ short AnmCHallPara_Opt[0];
// /* data 18b740 */ short AnmCHallPara_RepRet[0];
// /* data 18b748 */ short AnmCHallPara_Rep[0];
// /* data 18b750 */ short AnmCHallFphs_OptRet[0];
// /* data 18b758 */ short AnmCHallFphs_Opt[0];
// /* data 18b760 */ short AnmCHallFphs_RepRet[0];
// /* data 18b768 */ short AnmCHallFphs_Rep[0];
// /* data 18b770 */ short AnmCHallChar_Log[0];
// /* data 18b780 */ short AnmCHallChar_Opt[0];
// /* data 18b790 */ short AnmCHallChar_Rep[0];
// /* sdata 399748 */ static u_char *UserName_InitialStr;
// /* sdata 39974c */ static u_char *UserName_InitialStr2;
// /* data 18b7a0 */ static u_char UserName_AsciiSetB[41];
// /* data 18b7d0 */ static u_char UserName_AsciiSetS[41];
// /* data 18b800 */ USERNAME_CSET UserName_CharSet[0];
// /* data 18b810 */ static u_char *TeachersName_Tbl[0];
// /* sdata 39975c */ static u_char *UserName_RankingNoSave;
// /* data 18b830 */ static char *_MONTH_STR[0];
// /* data 18b880 */ static MAPBGM MapBgmTbl[0];
// /* data 18b908 */ static TSVOICE_TBL TsVoiceTbl[0];
// /* data 18bac8 */ static u_short FussenWAIT0[0];
// /* data 18bad8 */ static u_short FussenWAIT1[0];
// /* data 18bae8 */ static u_short LoadConfLst[0];
// /* data 18baf8 */ static TSVSNDSEQ VSNDSEQ_Tbl[0];
/* data 18bb10 */ extern TSTEX_TBL TexTable[]; /* static */
// /* data 18bcb0 */ static PATPOS PAT_ALERT_WIN_ABOVE;
// /* data 18bcc0 */ static PATPOS PAT_ALERT_WIN_CENTER;
// /* data 18bcd0 */ static PATPOS PAT_ALERT_WIN_BELOW;
// /* data 18bce0 */ static PATPOS PAT_ALERT_WIN_FFACE[0];
// /* data 18bd10 */ static PATPOS SAVE_MENU_SELPAT[0];
// /* data 18bd28 */ static PTPOS SAVEWZoom_CXY[0];
// /* data 18bd30 */ static PATPOS LG_SCROLL_MARK[0];
// /* data 18bd48 */ static PATPOS RP_SCROLL_MARK[0];
// /* data 18bd60 */ static PATPOS LLG_SCROLL_MARK[0];
// /* data 18bd78 */ static PATPOS CSSLASH_MARK;
// /* data 18bd88 */ static STRPOS PAGENO_StrCOD[0];
// /* data 18bd98 */ static PTPOS CellCusPos[0];
// /* data 18bdb8 */ static STRPOS LOGS_StrCOD[0];
// /* data 18bde0 */ static STRPOS LOGL_StrCOD[0];
// /* data 18be08 */ static STRPOS REPLAY_StrCOD[0];
// /* data 18be38 */ static STRPOS VSREPLAY_StrCOD[0];
// /* data 18be78 */ static PATPOS VS_MARK;
// /* data 18be88 */ static PATPOS VS_WINMARK1;
// /* data 18be98 */ static PATPOS VS_WINMARK2;
// /* data 18bea8 */ static PATPOS LG_NEWDATA_MARK;
// /* data 18beb8 */ static PATPOS RP_NEWDATA_MARK;
// /* data 18bec8 */ static PATPOS STGCNameBox[0];
// /* data 18bf08 */ static PATPOS STGCNameBoxOK;
// /* data 18bf18 */ static PATPOS VS1PNameBox[0];
// /* data 18bf58 */ static PATPOS VS1PNameBoxOK;
// /* data 18bf68 */ static PATPOS VS2PNameBox[0];
// /* data 18bfa8 */ static PATPOS VS2PNameBoxOK;
// /* data 18bfb8 */ static MNOPT_OBJ MNOptObj_Lang[0];
// /* data 18bfd8 */ static MNOPT_OBJ MNOptObj_Subt[0];
// /* data 18bff8 */ static MNOPT_OBJ MNOptObj_Vibr[0];
// /* data 18c018 */ static MNOPT_OBJ MNOptObj_Oneb[0];
// /* data 18c038 */ static PATPOS MNOptMiniFrm[0];
// /* data 18c068 */ static PATPOS MNOptLRBtn[0];
// /* data 18c0c8 */ static PATPOS PopMenuSel_Pat[0];
// /* data 18c108 */ static PATPOS VSComMenuSel_Pat[0];
// /* data 18c138 */ static PATPOS VSComMenuSelH_Pat[0];
// /* data 18c168 */ static PATPOS SIRanking_Pat[0];
// /* data 18c180 */ static PATPOS Ranking_PatScroll[0];
// /* data 18c198 */ static PATPOS RankSISTNo_PAT[0];
// /* data 18c208 */ static PATPOS VSL1Ranking_Pat[0];
// /* data 18c248 */ static PATPOS VSL2Ranking_Pat[0];
// /* data 18c288 */ static PATPOS VSL3Ranking_Pat[0];
// /* data 18c2c8 */ static PATPOS VSL4Ranking_Pat[0];
// /* data 18c308 */ static PATPOS *VSRanking_PatTbl[0];
// /* data 18c318 */ static PATPOS RankVSSTNo_PAT[0];
// /* data 18c388 */ static STRPOS SRanking_Str[0];
// /* data 18c3a0 */ static STRPOS VRanking_Str[0];
// /* data 18c3b8 */ static PTPOS PopMenu_Pos[0];
// /* data 18c3c0 */ static PTPOS Ranking_Pos[0];
// /* data 18c3c8 */ static POPRNK_PPOS PopRnk_pPos[0];
// /* data 18c420 */ static int PopRnkPos_No[0][9];
// /* data 18c4b0 */ static int PopBubblePat_No[0][9];
// /* data 18c540 */ static PTPOS POPWZoom_CXY[0];
// /* data 18c588 */ static PTPOS JUKEBOX_Pos[0];
// /* data 18c5b0 */ static PATPOS JUKEJKT_Pat[0];
// /* data 18c628 */ static float JUKEWAV_INITBL[0];
// /* data 18c650 */ static PATPOS JUKEJKT_PatS[0];
// /* data 18c660 */ static PATPOS JUKEREC_Pat[0];
// /* data 18c6d8 */ static PATPOS JUKEREC_PatS[0];
// /* data 18c6e8 */ static u_int HosiColor[0][8];
// /* data 18c748 */ HOSI_TYPE hTypeTable[0];
/* sdata 3997bc */ extern TSTEX_INF *tblTex; /* static */
// /* data 18c8e0 */ static u_int RPPadBit[0];
// /* data 18c8f0 */ static MCDATA_TBL McVoiceTbl[0];
// /* data 18c9a8 */ static MCDATA_TBL McFaceTbl[0];
/* sdata 3997c0 */ extern int UserList_Sw; /* static */
/* sdata 3997c4 */ extern int OptionList_Sw; /* static */
/* sdata 3997c8 */ extern int PopMenu_Sw; /* static */
/* sdata 3997cc */ extern int SaveMenu_Sw; /* static */
/* sdata 3997d0 */ extern int JukeMenu_Sw; /* static */
// /* data 18ca60 */ static USERLISTTYPE_TABLE ULTypeT_CITY_STGCLR;
// /* data 18ca70 */ static USERLISTTYPE_TABLE ULTypeT_CITY_REPLAY;
// /* data 18ca80 */ static USERLISTTYPE_TABLE ULTypeT_SAVE_LOG;
// /* data 18ca90 */ static USERLISTTYPE_TABLE ULTypeT_SAVE_REPLAY;
// /* data 18caa0 */ static int POPBtn2Sel[0];
// /* data 18cab8 */ static int POPSel2Btn[0];
// /* data 18cad0 */ static int Pop_CmpMesNo[0];
// /* data 18cae8 */ static int POPSel2BtnDir[0];
// /* data 18cb00 */ static int SaveMenu_CmpMesNo[0];
// /* data 18cb08 */ static BGM_TABLE JukeBgmTbl[0];
// /* data 18cb58 */ static int JukeMenu_CmpMesNo[0];
// /* data 18cb80 */ static MNOPT_SELINF OptionSelTbl[0];
// /* data 18cbc0 */ static USERLIST_TYPE UserListTbl[0];
// /* sdata 399820 */ static int _TexFunc;
/* sdata 399824 */ extern HOSI_OBJ *HOSIObj; /* static */
/* bss 1c77ac0 */ extern TSREPPAD menuPadState[2][4]; /* static */
// /* bss 1c77ae0 */ static TSSND_CHAN TsSndChan[15];
/* bss 1c77c10 */ extern BGMSTATE TsBGMState; /* static */
/* sbss 399b18 */ extern P3GAMESTATE *pP3GameState; /* static */
/* sbss 399b1c */ extern int _bMapCaptureReq; /* static */
/* sbss 399b20 */ extern int _MNwaitTime; /* static */
/* data 18cc38 */ extern MN_SCENE MNS_StageMap;
/* bss 1c77cb0 */ extern MN_SCENE MNS_StageMap2; /* static */
/* bss 1c78e08 */ extern MN_SCENE MNS_CityHall; /* static */
/* bss 1c79f60 */ extern MN_SCENE MNS_OptCounter; /* static */
/* bss 1c7b0b8 */ extern MN_SCENE MNS_RepCounter; /* static */
/* bss 1c7c210 */ extern MN_SCENE MNS_StgCounter[2]; /* static */
/* bss 1c7e4b8 */ extern MN_SCENE MNS_JimakuBak; /* static */
// /* sbss 399b24 */ static int CurMapOldFlg;
// /* sbss 399b28 */ static int CurMapNo;
// /* sbss 399b2c */ static int CurMapBakFlg;
// /* sbss 399b30 */ static int CurMapState;
/* sbss 399b34 */ extern USER_DATA *UserWork; /* static */
/* sbss 399b38 */ extern P3MC_STAGERANK *pCStageRank; /* static */
// /* bss 1c7f610 */ static P3MC_RANKSCORE CurRankScore;
/* sbss 399b3c */ extern P3MC_USRLST *UserLst; /* static */
/* sbss 399b40 */ extern MN_USERLST_WORK *UserDispWork; /* static */
/* bss 1c7f628 */ extern CURFILEINFO CurFileInfo; /* static */
/* bss 1c7f640 */ extern TsUSERPKT MnPkt; /* static */
/* bss 1c7f6e0 */ extern TsUSERPKT MnLPkt; /* static */
// /* bss 1c7f780 */ static sceGifPacket FPacket;
// /* sdata 399828 */ MAP_TIME MapTime;
// /* sbss 399b44 */ static int UCheckLoadError;
// /* sbss 399b48 */ static int UCheckSaveError;
/* sbss 399b4c */ extern int subStatus; /* static */
/* sbss 399b50 */ extern int ret; /* static */
/* sbss 399b54 */ extern int errorNo; /* static */
/* sbss 399b58 */ extern int waitTime; /* static */
// /* sbss 399b5c */ static MCRWDATA_HDL *pGameData;
// /* bss 1c7f790 */ static MCMES_WORK MCMesWork;
// /* bss 1c7f7c8 */ static CMPMES_WORK CmpMesWork;
// /* bss 1c7f7e0 */ static RANKLIST RankLst[20];
// /* bss 1c7f970 */ static POPUP_MENU PopupMenu;
// /* bss 1c7fa88 */ static SAVE_MENU SaveMenu;
// /* bss 1c7fb48 */ static JUKE_MENU JukeMenu;
// /* bss 1c80f60 */ static OPTION_MENU OptionMenu;
// /* bss 1c80fb0 */ static USERLIST_MENU UserListMenu;
// /* bss 1c810c8 */ static SCFADE ScFade;

static int TsGetMenuPadIsRepeat(int no, int npad);
/* static */ void  TSSNDPLAY(int n);
/* static */ void  TSSNDSTOP(int chan);
/* static */ void  TSSNDMASK_CHAN(int chan, int mskflag);
/* static */ void  TSSND_SKIPSTOP(int n);
/* static */ void  TSSND_SKIPPLAY(int n);
/* static */ int   TSSND_CHANISSTOP(int chan);
/* static */ void  tsBGMONEPlay(int no);
/* static */ void  tsBGMONEStop(int no);
/* static */ void  tsBGMONEVol(int no, int vol);
/* static */ void  tsBGMONETop(int no, int vol);
/* static */ void  tsBGMONEflow(void);
/* static */ void  tsBGMONEPause(int flg);
/* static */ void  TsBGMInit(void);
/* static */ void  TsBGMPlay(int no, int time);
/* static */ void  TsBGMStop(int time);
/* static */ void  TsBGMMute(int time);
/* static */ int   TsBGMLoadCheck(void);
/* static */ void  TsBGMPause(int flg);
/* static */ void  TsBGMPoll(void);
/* static */ void* TsCmnPkOpen(sceGifPacket *pgifpk);
/* static */ void  TsCmnPkClose(sceGifPacket *pgifpk, void *pk, int pri);
/* static */ void  TsClearMenuPad(int no);
/* static */ void  TsGetMenuPad(int no, u_int *getpad);
/* static */ void  TsSndFlow(int flg);
/* static */ int   TSNumMov(int cn, int dn, int scale);
static int TSLOOP(int no, int max);
static int TSLIMIT(int no, int min, int max);
/* static */ int   TsMENU_GetMapNo(int *psize);
/* static */ void  TsMENU_GetMapTimeState(int flg);
/* static */ void  TsSetScene_Map(MN_SCENE *pScene, int mapNo, int tflg, int bFocus);
/* static */ void  TsSet_ParappaCapColor(void);
/* static */ void  TsClearSet(P3GAMESTATE *pstate);
/* static */ void  TsCheckEnding(P3GAMESTATE *pstate);
/* static */ void  TsSetRankingName(P3MC_STAGERANK *pRankTop, u_char *name);
/* static */ void  TsSetRanking2UData(USER_DATA *puser, P3MC_STAGERANK *wkRank);
/* static */ void  TsSetSaveData(MCRWDATA_HDL *pDataW, int mode, USER_DATA *puser);
/* static */ void  TsRestoreSaveData(MCRWDATA_HDL *pDataW, int mode);
/* static */ int   TsRanking_Set(void);
/* static */ int   TsCheckTimeMapChange(void);
static int TsMemCardCheck_Flow(int flg, u_int tpad);
/* static */ int   TsMap_Flow(int flg, u_int tpad, u_int tpad2);
/* static */ void  TsMakeUserWork(int mode);
/* static */ void  TsSaveSuccessProc(void);
/* static */ int   MpSave_Flow(int flg, u_int tpad, u_int tpad2);
/* static */ int   MpCityHall_Flow(int flg, u_int tpad, u_int tpad2);
/* static */ void  MpCityHallParaStart(int pos);
/* static */ void  MpCityHallFPHSSoundMask(int flg);
/* static */ int   MpCityHallFPHSMove(int pos, int fpos);
/* static */ void  MpCityHallFPHOK(int flg);
/* static */ void  MpCityHallCharPosSet(int pos);
/* static */ int   MpPopMenu_Flow(int flg, u_int tpad);
/* static */ int   MpMapMenu_Flow(int flg, MAPPOS *mpw, u_int tpad);
/* static */ int   _MapGetMovableDir(MAPPOS *mpw);
/* static */ int   McErrorMess(int err);
static void McInitFlow(void);
/* static */ int   McStartCheckFlow(int flg);
/* static */ int   McUserCheckFlow(int type, int mode, int *bError);
/* static */ int   McUserSaveFlow(USER_DATA *puser);
/* static */ int   McUserLoadFlow(int fileNo, int mode, int bBroken);
/* static */ void  TsMCAMes_Init(void);
/* static */ int   TsMCAMes_GetSelect(void);
/* static */ int   TsMCAMes_IsON(void);
/* static */ void  TsMCAMes_Flow(u_int tpad);
/* static */ void  TsMCAMes_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ void  TsCMPMes_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ void  TsANIME_Init(ANIME_WK *wk);
/* static */ int   TsANIME_Poll(ANIME_WK *wk);
/* static */ void  TsANIME_Start(ANIME_WK *wk, int state, int tim);
/* static */ int   TsANIME_GetRate(ANIME_WK *wk, float *rt0, float *rt1, float *rt2);
/* static */ void  _TsSortSetRanking(P3MC_RANKSCORE **ptRank, int n, P3MC_RANKSCORE *pRank, int bNameCmp);
/* static */ RANKLIST* TsGetRankingList(int flag, int vsLev, int stageNo, int *nrank);
/* static */ int   TsPopMenu_Flow(int flg, u_int tpad);
/* static */ void  TsPopMenu_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ int   TsSaveMenu_Flow(int flg, u_int tpad);
/* static */ void  TsSaveMenu_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ void  TSJukeCDObj_Init(JUKECDOBJ *pw, int pno);
/* static */ void  _TsJkJacketPut(SPR_PKT pk, SPR_PRM *spr, JUKECDOBJ *pw, int px, int py, float zx, float rot, u_int abgr, u_int abgrs);
/* static */ void  _TsJkRecordPut(SPR_PKT pk, SPR_PRM *spr, JUKECDOBJ *pw, int px, int py, float zr, float rrot, u_int abgr, u_int abgrs);
/* static */ void  TSJukeCDObj_Draw(SPR_PKT pk, SPR_PRM *spr, JUKECDOBJ *pw, int px, int py, int anmtime);
/* static */ int   TsJukeIsObjAnime(int isComp);
/* static */ int   TsJukeObjAnime(int isOut);
/* static */ int   TsJukeObjAnime2(int isOut);
/* static */ int   _TsJKMoveCus(int *cx, int *cy, int mx, int my, JUKECDOBJ *cobj);
/* static */ void  _TsJKSetPadArrow(int sel, JUKECDOBJ *cobj);
/* static */ int   TsJukeMenu_Flow(int flg, u_int tpad);
/* static */ void  TsJukeMenu_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ void  TsCmnCell_CusorSET(CELLOBJ *obj);
/* static */ void  TsCmnCell_CusorON(CELLOBJ *obj);
/* static */ void  TsCmnCell_CusorOFF(CELLOBJ *obj);
/* static */ void  TsCmnCell_CusorSEL(CELLOBJ *obj);
/* static */ void  TsCmnCell_CusorMASK(CELLOBJ *obj);
/* static */ void  TsCmnCell_CusorDraw(SPR_PKT pk, SPR_PRM *spr, int n, CELLOBJ *obj, int ox, int oy, int CurColor);
/* static */ int   TsOption_Flow(int flg, u_int tpad);
/* static */ void  TsOption_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ int   TsUserList_GetCurFileNo(int *isBroken);
/* static */ int   TsUserList_IsGetFileSave(void);
/* static */ int   TsUserList_SortUser(void);
/* static */ void  TsUserList_SetCurUserData(USER_DATA *psrc);
/* static */ void  TsUserList_SetCurDispUserData(USER_DATA *psrc);
/* static */ void  TsUserList_SetCurFileNoCusor(int fileNo, u_int *fDate);
/* static */ void  TsUserList_SetType(USERLISTTYPE_TABLE *ptbl, int mode, int curTag);
/* static */ int   TsUserList_TagChangeAble(USERLIST_MENU *pfw, int *pno);
/* static */ int   TsUserList_SetCurTag(USERLIST_MENU *pfw, int no);
/* static */ int   TsUserList_Flow(int flg, u_int tpad, u_int tpad2);
/* static */ void  TsUserList_Draw(SPR_PKT pk, SPR_PRM *spr);
/* static */ void  NameSpaceCut(u_char *dst, u_char *src);
/* static */ void  TsUser_PanelDraw(SPR_PKT pk, SPR_PRM *spr, USER_DATA *user, int px, int py, int pflg, int isLog);
/* static */ void  TsNAMEINBox_SetName(NAMEINW *pfw, u_char *name);
/* static */ void  TsNAMEINBox_GetName(NAMEINW *pfw, u_char *name);
/* static */ int   TsNAMEINBox_Flow(int flg, NAMEINW *pfw, u_int tpad);
/* static */ void  TsNAMEINBox_Draw(SPR_PKT pk, SPR_PRM *spr, int px, int py, int isLog, NAMEINW *pfw, int side);
/* static */ void  TsSCFADE_Flow(int flg, int prm);
/* static */ void  TsSCFADE_Draw(SPR_PKT pk, SPR_PRM *spr, int prio);
/* static */ void  TsPatTexFnc(int flg);
/* static */ void  _TsPatSetPrm(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy);
/* static */ void  TsPatPut(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy);
static void TsPatGetSize(PATPOS *ppos, int *x, int *y, int *w, int *h);
/* static */ void  TsPatPutRZoom(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy, float zrate, float rot);
/* static */ void  TsPatPutMZoom(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy, float Zrx, float Zry, int mx, int my, float Crx, float Cry);
/* static */ void  TsPatPutSwing(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy, int mx, int my, float Crx);
static void TsPatPutUneri(SPR_PKT pk, SPR_PRM *spr, PATPOS *ppos, int ox, int oy, int mx, int my, float Crx, float Drt);
/* static */ void  TsCELBackInit(void);
/* static */ void  _TsCELBackObjDraw(SPR_PKT pk, SPR_PRM *spr, int sw, int sh, u_int *colTbl);
/* static */ void  TsHosiPut(SPR_PKT pk, SPR_PRM *spr, TSTEX_INF *ptex, float px, float py, float zrate, float rot);

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

void TsMenu_RankingClear(void) {
    int             i;
    P3MC_STAGERANK *pRank = pCStageRank;

    for (i = 0; i < 8; i++, pRank++) {
        memset(pRank, 0, sizeof(P3MC_STAGERANK));
    }
}

void TsMenu_Init(int iniflg, P3GAMESTATE *pstate) {
    int   i;
    void *ptim2;

    pP3GameState = pstate;

    PrSetStage(0);

    P3MC_SetCheckSaveSize(1, 0x8c, 0x188);
    P3MC_SetCheckSaveSize(2, 0xa0, 0x4528);

    if (!iniflg) {
        MENUSubt_PadFontSw(0);
        TsMemCardCheck_Flow(1, 0);
    } else {
        MENUSubt_PadFontSw(1);

        MNScene_Init(&MNS_CityHall, &Scene_CityHall, 1);
        MNScene_Init(&MNS_StageMap, &Scene_StageMap, 1);

        MNScene_Init(&MNS_OptCounter, &Scene_OptCounter, 0);
        MNScene_Init(&MNS_RepCounter, &Scene_RepCounter, 0);
        MNScene_Init(&MNS_JimakuBak, &Scene_JimakuBak, 0);

        MNScene_Init(&MNS_StgCounter[0], &Scene_StgCounterLoad, 0);
        MNScene_Init(&MNS_StgCounter[1], &Scene_StgCounterSave, 0);

        MNScene_DispSw(&MNS_OptCounter, 0);
        MNScene_DispSw(&MNS_RepCounter, 0);
        MNScene_DispSw(&MNS_JimakuBak, 0);

        for (i = 0; i < 2; i++) {
            MNScene_DispSw(&MNS_StgCounter[i], 0);
        }

        tblTex = (TSTEX_INF*)malloc(sizeof(TSTEX_INF) * 104);

        for (i = 0; i < 104; i++) {
            switch (TexTable[i].flg) {
            case 0:
                TsGetTm2HedTex(TexTable[i].fno, &tblTex[i]);
                break;
            case 1:
                ptim2 = GetIntAdrsCurrent(TexTable[i].fno);
                Tim2Trans(ptim2);
                TsGetTm2Tex(ptim2, &tblTex[i]);
                break;
            case 2:
                ptim2 = GetIntAdrsCurrent(TexTable[i].fno);
                TsGetTm2Tex(ptim2, &tblTex[i]);
                break;
            }
        }
    }

    UserLst = (P3MC_USRLST*)memalign(16, sizeof(P3MC_USRLST));
    memset(UserLst, 0, sizeof(P3MC_USRLST));
    P3MC_CheckChangeSet();

    UserDispWork = (MN_USERLST_WORK*)memalign(16, sizeof(MN_USERLST_WORK));
    memset(UserDispWork, 0, sizeof(MN_USERLST_WORK));

    TsInitUPacket(&MnPkt, NULL, 0x20000);
    TsInitUPacket(&MnLPkt, NULL, 0x2000);

    TsMCAMes_Init();
    TsMCAMes_SetMes(-1);
    TsCMPMes_SetMes(-1);

    TsClearMenuPad(0);
    TsClearMenuPad(1);

    TsSCFADE_Flow(1, 0);
    TsBGMInit();

    _bMapCaptureReq = 0;

    UserList_Sw = 0;
    OptionList_Sw = 0;
    PopMenu_Sw = 0;
    SaveMenu_Sw = 0;
    JukeMenu_Sw = 0;

    TsSndFlow(1);

    TsMENU_GetMapTimeState(1);
}

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

static int TsMemCardCheck_Flow(int flg, u_int tpad) {
    extern int state;
    extern int mesNo;
    int ret;

    if (flg == 1) {
        McInitFlow();
        state = 0;
        mesNo = -1;
        McStartCheckFlow(1);
        TsMCAMes_SetMes(-1);
        return 0;
    }

    switch (state) {
    case 0:
        ret = McStartCheckFlow(0);
        if (ret == 0) {
            state = 0x1000;
            return 0;
        }

        if (ret >= 0) {
            if (mesNo != ret) {
                mesNo = ret;

                switch (ret) {
                case 1:
                    TsMCAMes_SetMes(0x1030001);
                    break;
                case 2:
                    TsMCAMes_SetMes(0x1030000);
                    break;
                }
            }

            if (TsMCAMes_GetSelect()) {
                state = 0x1000;
            }
        }

        break;
    case 0x1000:
        if (TsSCFADE_Set(2, 0xf, 0)) {
            return 0;
        }
        TsMCAMes_SetMes(-1);
        McStartCheckFlow(2);
        P3MC_CheckChangeSet();
        _MNwaitTime = 30;
        state = 0x1010;
    /* fallthrough */
    case 0x1010:
        if (--_MNwaitTime <= 0) {
            return 1;
        }
        break;
    }

    return 0;
}

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

static void McInitFlow(void) {
    subStatus = 0;
    ret       = 0;
    errorNo   = 0;
    waitTime  = 0;
}

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

INCLUDE_ASM("menu/menusub", TsPatTexFnc);

INCLUDE_ASM("menu/menusub", _TsPatSetPrm);

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
