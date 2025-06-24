#ifndef MENUSUB_H
#define MENUSUB_H

#include "common.h"

#include "menu/menu.h"
#include "menu/menu_mdl.h"
#include "menu/pksprite.h"

#include <eetypes.h>

typedef struct { // 0x18
    /* 0x00 */ int wtim;
    /* 0x04 */ short tim;
    /* 0x06 */ short dir;
    /* 0x08 */ float px;
    /* 0x0c */ float py;
    /* 0x10 */ float vx;
    /* 0x14 */ float vy;
} HOSI_OBJ;

typedef struct { // 0x4
    /* 0x0 */ u_short state;
    /* 0x2 */ u_short time;
} TSREPPAD;

typedef struct { // 0xc
    /* 0x0 */ int texNo;
    /* 0x4 */ short x;
    /* 0x6 */ short y;
    /* 0x8 */ short w;
    /* 0xa */ short h;
} PATPOS;

typedef struct { // 0x18
    /* 0x00 */ u_long tex0;
    /* 0x08 */ float rUsize;
    /* 0x0c */ float rVsize;
    /* 0x10 */ u_int w;
    /* 0x14 */ u_int h;
} TSTEX_INF;

typedef struct { // 0xc
    /* 0x0 */ u_short chan;
    /* 0x2 */ u_short tapNo;
    /* 0x4 */ int lpTimeF;
    /* 0x8 */ int lpTime;
} MAPBGM;

typedef struct { // 0xc
    /* 0x0 */ u_short vol;
    /* 0x2 */ short bPause;
    /* 0x4 */ int tim;
    /* 0x8 */ MAPBGM *pbgm;
} BGMONE;

typedef struct { // 0xa0
    /* 0x00 */ u_short vol;
    /* 0x02 */ u_short state;
    /* 0x04 */ u_short ttim;
    /* 0x06 */ u_short ttim0;
    /* 0x08 */ u_short wtLoad;
    /* 0x0a */ u_short wtNo;
    /* 0x0c */ u_short wtTim;
    /* 0x0e */ u_short pad;
    /* 0x10 */ u_short chgReq;
    /* 0x12 */ u_short sndno;
    /* 0x14 */ u_short oldno;
    /* 0x16 */ u_short cstate;
    /* 0x18 */ u_short ctim;
    /* 0x1c */ BGMONE wbgm[11];
} BGMSTATE;

typedef struct { // 0x8
    /* 0x0 */ u_char date_day;
    /* 0x1 */ u_char date_month;
    /* 0x2 */ u_short date_year;
    /* 0x4 */ u_char date_pad;
    /* 0x5 */ u_char date_second;
    /* 0x6 */ u_char date_minute;
    /* 0x7 */ u_char date_hour;
} MAP_TIME;

typedef struct { // 0x14
    /* 0x00 */ u_int score;
    /* 0x04 */ u_char name[16];
} RANKLIST;

typedef struct { // 0x6
    /* 0x0 */ short bCur;
    /* 0x2 */ short time;
    /* 0x4 */ short cltm;
} POPCOFF;

typedef struct { // 0xa0
    /* 0x00 */ int onTNo;
    /* 0x04 */ int okTim;
    /* 0x08 */ int onTim;
    /* 0x0c */ int srTim;
    /* 0x10 */ int srTNo;
    /* 0x14 */ POPCOFF offinf[16];
    /* 0x74 */ u_short bDim[16];
    /* 0x94 */ float fswing;
    /* 0x98 */ u_int habgr;
    /* 0x9c */ u_int nabgr;
} POPCTIM;

typedef struct { // 0x14
    /* 0x00 */ int atim;
    /* 0x04 */ int aflg;
    /* 0x08 */ int atrn;
    /* 0x0c */ int atrn2;
    /* 0x10 */ int atrn3;
} ANIME_WK;

typedef struct { // 0x118
    /* 0x000 */ int state;
    /* 0x004 */ int exitflg;
    /* 0x008 */ int selno;
    /* 0x00c */ int btnNo;
    /* 0x010 */ int bSelRank;
    /* 0x014 */ int selLev;
    /* 0x018 */ int levMax;
    /* 0x01c */ int isSelLev;
    /* 0x020 */ ANIME_WK awork;
    /* 0x034 */ int isRnkWAnime;
    /* 0x038 */ float bpTim;
    /* 0x03c */ POPCTIM cani;
    /* 0x0dc */ int urTim;
    /* 0x0e0 */ int await;
    /* 0x0e4 */ int selrnkpg;
    /* 0x0e8 */ int isRankOn;
    /* 0x0ec */ int rankFlg;
    /* 0x0f0 */ int rStageNo;
    /* 0x0f4 */ int rVsLev;
    /* 0x0f8 */ int nRanking;
    /* 0x0fc */ RANKLIST *pRanking;
    /* 0x100 */ int nPPosSet;
    /* 0x104 */ int nPBubPat;
    /* 0x108 */ int nPageMax;
    /* 0x10c */ int nTagMax;
    /* 0x110 */ int nRankMax;
    /* 0x114 */ int rsline;
} POPUP_MENU;

void TsBGMChangePos(int no);

int _P3DATA_SIZE(int mode);

void TsGetTm2Tex(void *ptim2, TSTEX_INF *tex);
void TsGetTm2HedTex(int no, TSTEX_INF *tex);

float TSNumRBack(float rt, float bkrt);

void TsMENU_InitSystem(void);
void TsMENU_EndSystem(void);
void TsMenu_RankingClear(void);

void TsMenu_Init(int iniflg, P3GAMESTATE *pstate);
void TsMenu_End(void);

void TsMenu_InitFlow(P3GAMESTATE *pstate);
int  TsMenuMemcChk_Flow(void);
int  TsMenu_Flow(void);
void TsMenu_Draw(void);

int DateChgInt(u_int n);
void GetRankScoreID(MAP_TIME *mptim, u_int *dat);

void TsMENU_SetMapScreen(int mapNo);
int TsAnimeWait_withKeySkip(u_int tpad, MN_SCENE *scene, int ltim, u_int bnk);

void TsMCAMes_SetPos(int x, int y);
void TsMCAMes_SetMes(int no);

void TsCMPMes_SetPos(int x, int y);
void TsCMPMes_SetMes(int no);

void TsPopCusAOff(POPCTIM *pfw);
void TsPopCusDim(POPCTIM *pfw, int n, int flg);
void TsPopCusInit(POPCTIM *pfw, int curIdx);
void TsPopCusFlow(POPCTIM *pfw);
void TsPopCusPut(SPR_PKT pk, SPR_PRM *spr, int dflg, POPCTIM *pfw, int bPut, int idx, PATPOS *ppat, int px, int py);
int TsPUPCheckMove(int nbtn, int bank, POPCTIM *pfw);

void TsPopMenCus_Draw(SPR_PKT pk, SPR_PRM *spr, POPUP_MENU *pfw,int px, int py, u_int hicol, u_int nmcol, int dflg);

int TsSCFADE_Set(int flg, int num, int prio);

void _PkMCMsgPut(SPR_PKT pk, SPR_PRM *spr, int id, int x, int y, u_int abgr);
int _PkMCMsgGetLine(int id);

void _PkSubMsgPut(SPR_PKT pk, SPR_PRM *spr, int id, int x, int y, u_int abgr);

void TsMenu_CleanVram(int nFrm);
void TsMenu_CaptureVram(SPR_PKT pk, SPR_PRM *spr);

void TsSetCTransSpr(SPR_PKT pk, SPR_PRM *spr, int mx, int my, float zx, float zy);
void TsSetSLTransSpr(SPR_PKT pk, SPR_PRM *spr, int mx, int my, float zx);
void TsSetPNTransSpr(SPR_PKT pk, SPR_PRM *spr, int mx, int my, float wr, float dr);

void TsCELBackEnd(void);
int TsCELBackDraw(TsUSERPKT *UPacket, SPR_PRM *spr, int dispSw, int colNo);

#endif