#ifndef MENU_MDL_H
#define MENU_MDL_H

#include "common.h"

#include <eetypes.h>
#include <libgraph.h>

typedef struct { // 0x3c
    /* 0x00 */ int fn_mdl;
    /* 0x04 */ u_short fn_anmm[10];
    /* 0x18 */ u_short fn_anmp[18];
} MN_MDLTBL;

typedef struct { // 0x8
    /* 0x0 */ MN_MDLTBL *pmtbl;
    /* 0x4 */ int *pctbl;
} MN_SCENETBL;

typedef struct { // 0x4
    /* 0x0 */ u_short cflg;
    /* 0x2 */ u_short no;
} MNANM_COBJ;

typedef struct { // 0x20
    /* 0x00 */ u_char kind;
    /* 0x01 */ u_char aTimNo;
    /* 0x02 */ short stime;
    /* 0x04 */ short etime;
    /* 0x06 */ u_char aSpeed;
    /* 0x07 */ u_char pad;
    /* 0x08 */ MNANM_COBJ anmCobj[6];
} MNANM_TBL;

typedef struct { // 0x14
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0c */ float dist;
    /* 0x10 */ float rly;
} PRPOS;

typedef struct { // 0x8
    /* 0x0 */ PRPOS *ppos;
    /* 0x4 */ int npos;
} PRPROOT;

typedef struct { // 0x90
    /* 0x00 */ sceGifTag giftag;
    /* 0x10 */ sceGsDrawEnv1 denv1;
} DRAWENV_TAG12;

typedef struct { // 0x8c
    /* 0x00 */ void *spm;
    /* 0x04 */ void *spa_m[10];
    /* 0x2c */ void *spa_p[18];
    /* 0x74 */ u_short dspSw;
    /* 0x76 */ u_short MAniNo;
    /* 0x78 */ u_short PAniNo;
    /* 0x7a */ u_short PMovNo;
    /* 0x7c */ u_short MvTblNo;
    /* 0x7e */ u_short MSpaNo;
    /* 0x80 */ u_short PSpaNo;
    /* 0x82 */ u_short bABlend;
    /* 0x84 */ float ablend_rate;
    /* 0x88 */ float ablend_speed;
} MN_HMDL;

typedef struct { // 0x1154
    /* 0x0000 */ u_short isDisp;
    /* 0x0002 */ u_short pad;
    /* 0x0004 */ void *scene;
    /* 0x0008 */ int nmdl;
    /* 0x000c */ MN_HMDL mdl[30];
    /* 0x1074 */ int ncam;
    /* 0x1078 */ void *spc[14];
    /* 0x10b0 */ int time[10];
    /* 0x10d8 */ void *anime[10];
    /* 0x1100 */ void *cntani[10];
    /* 0x1128 */ int speed[10];
    /* 0x1150 */ short CSpcNo;
    /* 0x1152 */ short CAniNo;
} MN_SCENE;

void MNSceneMusicFitTimerClear(void);
void MNSceneMusicFitTimerFrame(void);
int MNSceneGetMusicFitTimer(void);
void MNScene_Init(MN_SCENE *pshdl, MN_SCENETBL *tbl, int bFocus);
void MNScene_End(MN_SCENE *pshdl);
void MNScene_Draw(MN_SCENE *pshdl);
void MNScene_DispSw(MN_SCENE *pshdl, int sw);
void MNScene_SetAnimete(MN_SCENE *pshdl);
u_int MNScene_StartAnime(MN_SCENE *pshdl, int no, MNANM_TBL *anime);
void MNScene_ContinueAnime(MN_SCENE *pshdl, int no, MNANM_TBL *anime);
void MNScene_StopAnime(MN_SCENE *pshdl, int no);
void MNScene_ExecAnime(MN_SCENE *pshdl);
void MNScene_CopyState(MN_SCENE *pdhdl, MN_SCENE *pshdl);
void MNScene_CopyStateMdl(MN_SCENE *pdhdl, MN_SCENE *pshdl);
void MNScene_SetAnimeSpeed(MN_SCENE *pshdl, int nAnime, int speed);
void MNScene_SetAnimeEnd(MN_SCENE *pshdl);
void MNScene_SetAnimeBankEnd(MN_SCENE *pshdl, u_int bnk);
int MNScene_isAnime(MN_SCENE *pshdl, int ltim);
int MNScene_isAnimeBank(MN_SCENE *pshdl, int ltim, u_int bnk);
int MNScene_isSeniAnime(MN_SCENE *pshdl);
int MNScene_ModelDispSw(MN_SCENE *pshdl, int nmdl, int bsw);

#endif