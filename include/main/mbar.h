#ifndef MBAR_H
#define MBAR_H

#include "common.h"

#include "main/scrctrl.h"
#include "main/sprite.h"

#include <eetypes.h>
#include <libgifpk.h>

typedef enum {
    NIKO_KAGE = 0,
    NIKO_HALF = 1,
    NIKO_MARU = 2,
    NIKO_SKIP = 3,
    NIKO_MAX = 4
} NIKO_ENUM;

typedef enum {
    MBAR_NONE = 0,
    MBAR_TEACHER = 393,
    MBAR_PARAPPA = 745,
    MBAR_TEACHER_HOOK = 393,
    MBAR_PARAPPA_HOOK = 1769,
    MBAR_TEACHER_VS = 105,
    MBAR_PARAPPA_VS = 553,
    MBAR_BOXY_VS = 2089
} MBAR_REQ_ENUM;

typedef enum {
    VSMT_NONE = 0,
    VSMT_UP = 1,
    VSMT_DW = 2,
    VSMT_MAX = 3
} VS_MV_TYPE;

typedef enum {
    MBC_NONE = 0,
    MBC_OTEHON_TOP = 1,
    MBC_TR = 1,
    MBC_CI = 2,
    MBC_XX = 3,
    MBC_SQ = 4,
    MBC_LL = 5,
    MBC_RR = 6,
    MBC_SP = 7,
    MBC_OTEHON_M_TOP = 8,
    MBC_M_TR = 8,
    MBC_M_CI = 9,
    MBC_M_XX = 10,
    MBC_M_SQ = 11,
    MBC_M_LL = 12,
    MBC_M_RR = 13,
    MBC_M_SP = 14,
    MBC_OTEHON_BW_TOP = 15,
    MBC_BW_TR = 15,
    MBC_BW_CI = 16,
    MBC_BW_XX = 17,
    MBC_BW_SQ = 18,
    MBC_BW_LL = 19,
    MBC_BW_RR = 20,
    MBC_BW_SP = 21,
    MBC_BALL = 22,
    MBC_STAR = 23,
    MBC_TEA_FACE_TOP = 24,
    MBC_NONECUR = 24,
    MBC_PARA = 25,
    MBC_BOXY = 26,
    MBC_STAGE1 = 27,
    MBC_STAGE2 = 28,
    MBC_STAGE3 = 29,
    MBC_STAGE4 = 30,
    MBC_STAGE5 = 31,
    MBC_STAGE6 = 32,
    MBC_STAGE7 = 33,
    MBC_STAGE8 = 34,
    MBC_FLASH = 35,
    MBC_GLINE_P = 36,
    MBC_GLINE_T = 37,
    MBC_MAX = 38
} MBC_ENUM;

typedef enum {
    MBWINDOW_NORMAL = 0,
    MBWINDOW_UP = 1,
    MBWINDOW_DOWN = 2,
    MBWINDOW_MAX = 3
} MBWINDOW_ENUM;

typedef struct { // 0x38
    /* 0x00 */ u_long GsTex0;
    /* 0x08 */ u_long GsTex1;
    /* 0x10 */ u_int GsRegs;
    /* 0x14 */ short x;
    /* 0x16 */ short y;
    /* 0x18 */ u_short w;
    /* 0x1a */ u_short h;
    /* 0x1c */ u_short u;
    /* 0x1e */ u_short v;
    /* 0x20 */ u_short cx;
    /* 0x22 */ u_short cy;
    /* 0x24 */ u_short alpha;
    /* 0x28 */ float scalex;
    /* 0x2c */ float scaley;
    /* 0x30 */ u_char kido[3];
} EX_CHAR_DISP;

typedef struct { // 0x8
    /* 0x0 */ u_short xp;
    /* 0x2 */ u_short yp;
    /* 0x4 */ NIKO_ENUM niko_enum;
} NIKO_CHAN_STR;

typedef struct { // 0x24
    /* 0x00 */ MBAR_REQ_ENUM mbar_req_enum;
    /* 0x04 */ TAPSET *tapset_pp;
    /* 0x08 */ int current_time;
    /* 0x0c */ int scr_tap_memory_cnt;
    /* 0x10 */ SCR_TAP_MEMORY *scr_tap_memory_pp;
    /* 0x14 */ int lang;
    /* 0x18 */ int tapdat_size;
    /* 0x1c */ TAPDAT *tapdat_pp;
    /* 0x20 */ GUI_CURSOR_ENUM gui_cursor_enum;
} MBAR_REQ_STR;

typedef struct { // 0x18
    /* 0x00 */ MBC_ENUM mbc_enum;
    /* 0x04 */ int xp;
    /* 0x08 */ int yp;
    /* 0x0c */ float sclx;
    /* 0x10 */ float scly;
    /* 0x14 */ u_char r;
    /* 0x15 */ u_char g;
    /* 0x16 */ u_char b;
    /* 0x17 */ u_char a;
} MBARR_CHR;

typedef struct { // 0x28
    /* 0x00 */ MBC_ENUM mbc_enum;
    /* 0x04 */ int xp;
    /* 0x08 */ int yp;
    /* 0x0c */ int xp2;
    /* 0x10 */ int yp2;
    /* 0x14 */ int ofsx;
    /* 0x18 */ int ofsy;
    /* 0x1c */ int ofsx2;
    /* 0x20 */ int ofsy2;
    /* 0x24 */ u_char r;
    /* 0x25 */ u_char g;
    /* 0x26 */ u_char b;
    /* 0x27 */ u_char a;
} MBARR_CHR2;

void examCharSet(EX_CHAR_DISP *ecd_pp, sceGifPacket *gifpk_pp);

void MbarMemberClear(int stg);

void examCharBasic(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp);
void examCharScaleSet(EX_CHAR_DISP *ecd_pp, float scx, float scy);
void examCharCltSet(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp);
void examCharPosSet(EX_CHAR_DISP *ecd_pp, int xp, int yp);
void examCharUVWHSet(EX_CHAR_DISP *ecd_pp, u_short u, u_short v, u_short w, u_short h);
void examCharAlphaSet(EX_CHAR_DISP *ecd_pp, u_short on_off);
void examCharKidoSet(EX_CHAR_DISP *ecd_pp, u_char rc, u_char gc, u_char bc);

void MbarNikoHookUse(void);
void MbarNikoVsUse(void);
void MbarNikoUnUse(void);
void MbarNikoSet(int num, int ofs);

void MbarHookUseInit(void);
void MbarHookUnUse(void);
void MbarHookUseOK(void);
void MbarHookUseNG(void);

void vsAnimationInit(void);
void vsAnimationReq(int ply, long scrMoto, long scrSaki, VS_MV_TYPE vt);
void vsAnimationReset(int ply, long scr);

void metFrameInit(void);
void conditionFrameInit(void);

void ExamDispInit(void);
void ExamDispPlySet(GLOBAL_PLY *ply, int pos);
void ExamDispReq(int ply, int plmi);
void ExamDispReset(void);
float examScore2Level(long score);
void examNumDisp(sceGifPacket *ex_gif_pp, long score, short x, short y, int keta, u_char *coldat_pp, int plmi);
void ExamDispSet(void);
void ExamDispSubt(void);

void MbarInit(int stg);
void MbarReset(void);
void MbarReq(MBAR_REQ_ENUM mm_req, TAPSET *ts_pp, int curr_time, SCR_TAP_MEMORY *tm_pp, int tm_cnt, int lang, int tapdat_size, TAPDAT *tapdat_pp, GUI_CURSOR_ENUM guic);
void MbarSetCtrlTime(int mctime);

void MbarGifInit(void);
void MbarGifTrans(int pri);

void MbarCharSet(MBARR_CHR *mb_pp);
void MbarCharSet2(MBARR_CHR2 *mb_pp);

void MbarWindowSet(MBWINDOW_ENUM wenum);

void MbarSclRotMake(MBARR_CHR *mbarr_pp, int mbtime);
void MbarGuideLightMake(MBARR_CHR *mbarr_pp, int mbtime);
int MbarFlashMake(MBARR_CHR *mbarr_pp, MBARR_CHR *mbarr_moto_pp, int mbtime, int fltype);

void MbarBackSet(MBAR_REQ_STR *mr_pp);

void MbarDisp(void);

/* Scene control programs */
int MbarDispScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
int MbarDispSceneDraw(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
int MbarDispSceneVsDraw(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
/* Scene control programs end */

void MbarDispSceneVsDrawInit(void);

/* Scene control programs */
int MbarDispGuiScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
int MbarDispGuiSceneMbarArea(void *para_pp, int frame, int first_f, int useDisp, int drDisp);
/* Scene control programs end */

TIM2_DAT* lessonTim2InfoGet();
TIM2_DAT* lessonCl2InfoGet(SCRRJ_LESSON_ROUND_ENUM type);

void MbarDemoCharDisp(void);

#endif