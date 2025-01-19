#ifndef MENUSUB_H
#define MENUSUB_H

#include "common.h"
#include <eetypes.h>

#include "os/syssub.h"

#include "menu/menu.h"
#include "menu/pksprite.h"

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

void TsMENU_InitSystem(void);
void TsMENU_EndSystem(void);
void TsMenu_RankingClear(void);

void TsMenu_Init(int iniflg, P3GAMESTATE *pstate);
void TsMenu_End(void);

void TsMenu_InitFlow(P3GAMESTATE *pstate);
int  TsMenuMemcChk_Flow(void);
int  TsMenu_Flow(void);

void TsMenu_Draw(void);

#endif