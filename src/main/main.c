#include "main/main.h"

#include "dbug/dbgmsg.h"
#include "dbug/dbug.h"

#include "os/cmngifpk.h"
#include "os/mtc.h"
#include "os/syssub.h"
#include "os/system.h"
#include "os/usrmem.h"

#include "main/cdctrl.h"
#include "main/cmnfile.h"
#include "main/etc.h"
#include "main/fadectrl.h"
#include "main/p3str.h"
#include "main/scrctrl.h"
#include "main/stdat.h"
#include "main/wipe.h"

#include "menu/menu.h"

#include "iop_mdl/tapctrl_rpc.h"

#include <libpad.h>

#include <stdio.h>

/* .sdata */
extern int urawaza_skip_bottun;
extern int urawaza_levelsel_bottun;

/* data */
extern DBG_SELECT_STR dbg_select_str; /* = {
    .debug_on      = 0,
    .use_line      = 0xFFFFFFFF,
    .score_updown  = 0,
    .non_play      = 0,
}; */

extern u_char *dbg_tbl_msg[]; /* = {
    "AUTO", "BASE",
    "LV1",
    "LV2",
    "LV3",
    "LV4",
    "LV5",
    "LV6",
    "LV7", 
    "LV8", 
    "LV9", 
    "LV10",
    "LV11", 
    "LV12", 
    "LV13", 
    "LV14", 
    "LV15", 
    "LV16",
}; */

extern u_char *dbg_score_msg[]; /* = {
    "OFF", "ON"
}; */

extern DBG_MODE_STR dbg_mode_str[]; /* = {
    {
        .msg_pp = "START",
        .set_pp = NULL,
        .min = 0,
        .max = 0,
        .selmsg_pp = NULL,
    },
    {
        .msg_pp = "TABLE",
        .set_pp = &dbg_select_str.use_line,
        .min = 0xFFFFFFFF,
        .max = 17,
        .selmsg_pp = dbg_tbl_msg,
    },
    {
        .msg_pp = "SCORE DBUG",
        .set_pp = &dbg_select_str.score_updown,
        .min = 0,
        .max = 2,
        .selmsg_pp = dbg_score_msg,
    },
    {
        .msg_pp = "NON PLAY",
        .set_pp = &dbg_select_str.non_play,
        .min = 0,
        .max = 2,
        .selmsg_pp = dbg_score_msg,
    },
}; */

extern int overlay_loadaddr;
extern int uramen_end_flag;

/* ,bss */
extern MENU_STR menu_str;

extern int first_f; /* mainStart */

INCLUDE_RODATA("main/main", D_00393810);
INCLUDE_RODATA("main/main", D_00393820);

INCLUDE_ASM("main/main", dbg_select_disp);
void dbg_select_disp(void);

INCLUDE_RODATA("main/main", D_00393840);
INCLUDE_RODATA("main/main", D_00393860);
INCLUDE_RODATA("main/main", D_00393880);
INCLUDE_RODATA("main/main", D_003938A8);
INCLUDE_RODATA("main/main", D_003938D0);
INCLUDE_RODATA("main/main", D_003938F8);

INCLUDE_RODATA("main/main", D_00393910);

#ifndef NON_MATCHING
INCLUDE_ASM("main/main", dummyPlay);
void dummyPlay(/* s0 16 */ int retTitle);
#else
static void dummyPlay(/* s0 16 */ int retTitle)
{
    /* s1 17 */ int mode;
    /* s2 18 */ int ret;
    /* -0x150(sp) */ long scoreTmp[2] = {};
    /* -0x140(sp) */ u_char *msgDmy[5] =
    {
        "TITLE   A or O or X EXIT",
        "REPLAY  A or O or X EXIT",
        "SINGLE  A .. COOL O .. GOOD  X .. NG",
        "VS MAN  A..1p win O..2p win X..EXIT",
        "VS COM  A or O .. CLEAR  X .. NG"
    };

    mode = 0; // 177

    if (retTitle == 0)
    {
        if (game_status.demo_flagG != 2) {
            // 179
            mode = 1;
            if (game_status.play_modeG == 0) {
                //DbgMsgInit();
            }
        }
    }

    game_status.bonusG = 0; // 185
    DbgMsgInit(); // 186

    while (1)
    {
        /* -0x120(sp) */ u_char msg_dat[80];
        /* -0xd0(sp) */ u_char *pmd[3] = { "SINGLE", "VS MAN", "VS COM" };
        /* -0xc0(sp) */ u_char *ptype[2] = { "NORMAL", "EASY" };
    }

#if 0
    /* s1 17 */ int mode;
    /* s2 18 */ int ret;
    /* -0x150(sp) */ long scoreTmp[2] = {};
    /* -0x140(sp) */ u_char *msgDmy[5] =
    {
        "TITLE   A or O or X EXIT",
        "REPLAY  A or O or X EXIT",
        "SINGLE  A .. COOL O .. GOOD  X .. NG",
        "VS MAN  A..1p win O..2p win X..EXIT",
        "VS COM  A or O .. CLEAR  X .. NG"
    };

    mode = 0;

    if (retTitle == 0)
    {
        mode = 0;

        if (game_status.demo_flagG == DEMOF_REPLAY)
        {
            mode = 1;
        }
        else if (game_status.play_modeG == PLAY_MODE_SINGLE)
        {
            mode = 2;
        }
        else if (game_status.play_modeG == PLAY_MODE_VS_MAN)
        {
            mode = 3;
        }
        else if (game_status.play_modeG != PLAY_MODE_SINGLE)
        {
            mode = 4;
        }
    }

    game_status.bonusG = 0;
    DbgMsgInit();

    while (1)
    {
        /* -0x120(sp) */ u_char msg_dat[80];
        /* -0xd0(sp) */ u_char *pmd[3] = { "SINGLE", "VS MAN", "VS COM" };
        /* -0xc0(sp) */ u_char *ptype[2] = { "NORMAL", "EASY" };

        msg_dat[0] = 0;

        if (pad[0].one & SCE_PADRright)
        {
            ret = 1;
        }
        else if (pad[0].one & SCE_PADRdown)
        {
            ret = 2;
        }
        else if (pad[0].one & SCE_PADRup)
        {
            ret = 3;
        }

        if (ret != 0)
            break;

        DbgMsgClear();
        DbgMsgSetSize(16, 10);
        DbgMsgSetColor(128, 128, 128);
        DbgMsgPrint(msgDmy[mode], 1780, 1968);

        if (mode == 1)
        {
            // sprintf(msg_dat, "STG:%d MODE:%s TYPE:%s ROUND:%d", mc_rep_str.play_stageS,pmd[mc_rep_str.play_modeS],ptype[mc_rep_str.play_typeS], mc_rep_str.roundS + TRND_R2);
            DbgMsgPrint(msg_dat, 1770, 2000);
        }
        else
        {
            if (pad[0].shot & SCE_PADL1)
                scoreTmp[0] += 10;
            if (pad[0].shot & SCE_PADL2)
                scoreTmp[0] -= 10;

            if (pad[0].shot & SCE_PADR1)
                scoreTmp[1] += 10;
            if (pad[0].shot & SCE_PADR2)
                scoreTmp[1] -= 10;

            if (scoreTmp[0] < 0)
                scoreTmp[0] = 0;
            if (scoreTmp[1] < 0)
                scoreTmp[1] = 0;

            sprintf(msg_dat, "STG:%d MODE:%s TYPE:%s ROUND:%d", game_status.play_stageG, pmd[game_status.play_modeG], ptype[game_status.play_table_modeG], game_status.roundG + TRND_R2);
            DbgMsgPrint(msg_dat, 1770, 2000);

            DbgMsgSetColor(0, 255, 0);
            DbgMsgPrint("SCORE 1P  L1 .. UP   L2 .. DOWN", 1770, 2020);
            DbgMsgPrint("SCORE 2P  R1 .. UP   R2 .. DOWN", 1770, 2032);

            DbgMsgSetColor(255, 255, 0);
            sprintf(msg_dat, "1P:%5d    2P:%5d", scoreTmp[0], scoreTmp[1]);
            DbgMsgPrint(msg_dat, 1770, 2046);
        }

        DbgMsgFlash();
        MtcWait(1);
    }

    switch (mode)
    {
    case 2:
        if ((ret == 1) || (ret == 3))
        {
            if (game_status.endingFlag == 1)
            {
                while (1)
                {
                    DbgMsgClear();
                    DbgMsgSetSize(16, 10);
                    DbgMsgSetColor(128, 128, 128);
                    DbgMsgPrint("ENDING   A or O or X  EXIT", 1800, 1968);
                    DbgMsgFlash();

                    if (pad[0].one & 0x70)
                        break;

                    MtcWait(1);
                }
            }
            else if (game_status.endingFlag - 2U < 3)
            {
                while (1)
                {
                    DbgMsgClear();
                    DbgMsgSetSize(16, 10);
                    DbgMsgSetColor(128, 128, 128);
                    DbgMsgPrint("BONUS GAME   A or O or X  EXIT", 1800, 1968);
                    DbgMsgFlash();

                    if (pad[0].one & 0x70)
                        break;

                    MtcWait(1);
                }
            }

            menu_str.sel_menu_enum = SEL_MENU_SAVE;
            mc_rep_str.play_modeS = game_status.play_modeG;
            mc_rep_str.roundS = game_status.roundG;
            mc_rep_str.play_typeS = game_status.play_typeG;
            mc_rep_str.play_table_modeS = game_status.play_table_modeG;
            mc_rep_str.play_stageS = game_status.play_stageG;

            if (ret != 1)
            {
                if (game_status.stClrCntCool[game_status.play_stageG] != -1)
                    game_status.stClrCntCool[game_status.play_stageG]++;
            }
            else
            {
                if (game_status.stClrCntGood[game_status.play_stageG] != -1)
                    game_status.stClrCntGood[game_status.play_stageG]++;
            }

            game_status.disp_level = (DISP_LEVEL)(ret == 1);

            game_status.disp_level = (ret == 1);

            game_status.scoreG[0] = scoreTmp[0];
            game_status.scoreG[1] = 0;
        }
        else
        {
            menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
            game_status.disp_level = DLVL_BAD; 
            game_status.scoreG[0] = 0;
            game_status.scoreG[1] = 0;
        }
        break;

    case 1:
        menu_str.sel_menu_enum = SEL_MENU_REPLAY;
        break;
    case 3:
        break;
    }

    /* SCOPE VARIABLES */
    // /* a1 5 */ u_int clrcnt;
    // /* a1 5 */ u_int clrcnt;
    // /* v1 3 */ GLOBAL_PLY *gply_pp;
    // /* v0 2 */ GLOBAL_PLY *gply_pp;
    // /* v0 2 */ GLOBAL_PLY *gply_pp;
#endif
}
#endif

extern char D_00393A00[]; /* rodata - "overlay module load in\n" */
extern char D_00393A18[]; /* rodata - "overlay module load out\n" */

int selPlayDisp(int sel_stage, int sel_disp, int firstf) {
    STDAT_DAT *stdat_dat_pp;
    int        ret = 0;

    ReportHeapUsage();
    printf("=== selPlayDisp stg:%d disp:%d ===\n", sel_stage, sel_disp);

    /* Load stage overlay */
    printf(D_00393A00);
    CdctrlRead(&stdat_rec[sel_stage].ovlfile, overlay_loadaddr, NULL);
    CdctrlReadWait();
    printf(D_00393A18);

    asm("sync.l");
    FlushCache(WRITEBACK_DCACHE);

    stdat_dat_pp = &stdat_rec[sel_stage].stdat_dat_pp[sel_disp];

    CdctrlGetFileSize(&stdat_dat_pp->intfile);
    CdctrlRead(&stdat_dat_pp->intfile, UsrMemAllocNext(), UsrMemAllocEndNext());
    CdctrlReadWait();

    cmnfTim2Trans();

    global_data.play_stageL = sel_stage;
    GlobalPlySet(&global_data, stdat_dat_pp->play_step, sel_stage);
    GlobalTimeInit(&global_data);
    GlobalSetTempo(&global_data, stdat_rec[sel_stage].stdat_dat_pp[sel_disp].tempo);

    ScrCtrlInit(stdat_dat_pp, (void*)UsrMemGetAdr(0));
    
    do {
        MtcWait(1);
    } while (!ScrCtrlInitCheck());

    if (!firstf) {
        while (!WipeEndCheck()) {
            MtcWait(1);
        }
    }
    
    if (firstf) {
        FadeCtrlReq(FMODE_BLACK_IN, 120);
    }

    ScrCtrlGoLoop();

    if (!firstf) {
        WipeOutReq();
    }

    PrSetPostureWorkArea(UsrMemAllocNext(), UsrMemAllocEndNext() - UsrMemAllocNext());
    DrawCtrlInit(stdat_dat_pp->ev_pp, global_data.draw_tbl_top, (void*)UsrMemGetAdr(0));

    PrSetPostureWorkArea(0, 0);
    DrawCtrlTimeSet(0);
    MtcWait(1);

    ReportHeapUsage();

    if (firstf) {
        MtcWait(120);
    }

    while (1) {
        MtcWait(1);

        if ((pad[0].one & SCE_PADstart) && WipeEndCheck()) {
            ret = 1;
            break;
        }

        if (ScrEndCheckScore()) {
            break;
        }

        if (ScrEndCheckFadeOut() && (global_data.demo_flagL == DEMOF_DEMO)) {
            ret = 2;
            break;
        }
    }

    if (ret == 2) {
        CdctrlSndFadeOutWait(120);
        ret = 0;
    }

    DrawCtrlQuit();
    CdctrlWP2SetVolume(0);
    CdctrlWp2FileEnd();
    ScrCtrlQuit();
    CdctrlMasterVolSet(0x3fff);

    return ret;
}

#ifndef NON_MATCHING
INCLUDE_ASM("main/main", SpHatChangeSub);
void SpHatChangeSub(void);
#else // TODO: fix rodata
static void SpHatChangeSub(void)
{
    PADD *pad_pp;

    hat_change_enum = HCNG_AUTO;

    if (game_status.roundG < TRND_R4)
        return;

    pad_pp = &pad[0];

    if (pad_pp->ana[1] < 0x40)
    {
        hat_change_enum = HCNG_R1;
    }
    else if (pad_pp->ana[0] >= 0xC0)
    {
        hat_change_enum = HCNG_R2;
    }
    else if (pad_pp->ana[1] >= 0xc0)
    {
        hat_change_enum = HCNG_R3;
    }
    else if (pad_pp->ana[0] < 0x40)
    {
        hat_change_enum = HCNG_R4;
    }

    {
        int           rt2t_r1[3]      = { 0xB6, 0xB7, 0xB8 };
        int           rt2t_r2[3]      = { 0xB9, 0xBA, 0xBB };
        int           rt2t_r3[3]      = { 0xBC, 0xBD, 0xBE };
        int           rt2t_r4[3]      = { 0xBF, 0xC0, 0xC1 };
        RT2TRANS_STR  rt2trans_str[4] = {{3, rt2t_r1}, {3, rt2t_r2}, {3, rt2t_r3}, {3, rt2t_r4}};
        
        RT2TRANS_STR *rt2trans_str_pp = &rt2trans_str[GetHatRound()];
        int           i;

        for (i = 0; i < rt2trans_str_pp->num; i++)
        {
            Tim2Trans(GetIntAdrsCurrent(rt2trans_str_pp->data_pp[i]));
        }
    }
}
#endif

extern char D_003996C0[]; /* sdata  - "DEBUG" */

int selPlayDispTitleDisp(int sel_stage, int sel_disp, int ovl_load) {
    STDAT_DAT *stdat_dat_pp;
    int        ret = 0;

    ReportHeapUsage();
    printf("=== selPlayDisp stg:%d disp:%d ===\n", sel_stage, sel_disp);

    if (ovl_load) {
        printf(D_00393A00);
        CdctrlRead(&stdat_rec[sel_stage].ovlfile, overlay_loadaddr, NULL);
        CdctrlReadWait();
        printf(D_00393A18);
    }

    asm("sync.l");
    FlushCache(WRITEBACK_DCACHE);

    stdat_dat_pp = &stdat_rec[sel_stage].stdat_dat_pp[sel_disp];

    global_data.play_stageL = sel_stage;
    GlobalPlySet(&global_data, stdat_dat_pp->play_step, sel_stage);
    GlobalTimeInit(&global_data);
    GlobalSetTempo(&global_data, stdat_rec[sel_stage].stdat_dat_pp[sel_disp].tempo);

    ScrCtrlInit(stdat_dat_pp, (void*)UsrMemGetAdr(0));

    do {
        MtcWait(1);
    } while (!ScrCtrlInitCheck());

    ScrCtrlGoLoop();
    WipeOutReq();

    PrSetPostureWorkArea(UsrMemAllocNext(), UsrMemAllocEndNext() - UsrMemAllocNext());
    DrawCtrlInit(stdat_dat_pp->ev_pp, global_data.draw_tbl_top, (void*)UsrMemGetAdr(0));
    PrSetPostureWorkArea(0, 0);
    DrawCtrlTimeSet(0);
    MtcWait(1);

    ReportHeapUsage();

    while (1) {
        MtcWait(1);

        if (ScrEndCheckTitle()) {
            ret = 1;
            break;
        }
        
        if (ScrEndCheckScore()) {
            break;
        }

        SpHatChangeSub();

        if (pad[0].one & SCE_PADLdown) {
            dbg_select_str.debug_on ^= 1;
        }

        if (dbg_select_str.debug_on) {
            sceGifPacket dbgPk;

            DbgMsgInit();
            DbgMsgClear();

            CmnGifOpenCmnPk(&dbgPk);
            DbgMsgClearUserPkt(&dbgPk);
            DbgMsgPrintUserPkt(D_003996C0, 1730, 1948, &dbgPk);
            CmnGifCloseCmnPk(&dbgPk, 7);
        }
    }

    if (ret != 0) {
        MtcWait(60);
    }

    DrawCtrlQuit();
    CdctrlWP2SetVolume(0);
    CdctrlWp2FileEnd();
    ScrCtrlQuit();
    return ret;
}

void xtrView(FILE_STR *file_str_pp) {
    int timer;
    int seek_top;

    timer = 0;

    CdctrlXTRset(file_str_pp, UsrMemAllocNext());
    seek_top = getTopSeekPos();

    CdctrlWP2Play();
    CdctrlWP2SetVolume(0x3fff);

    while (1) {
        MtcWait(1);

        if (pad[0].one & SCE_PADstart || CdctrlWP2PlayEndCheck() || timer >= 6540) {
            break;
        }

        CdctrlWp2GetSampleTmpBuf();

        timer = CdctrlWp2CdSample2Frame(CdctrlWp2GetSampleTmp() - seek_top);
        p3StrPoll(timer);

        timer++;
    }

    CdctrlWp2FileEnd();
    p3StrQuitSd();
}

void logoDispOne(SPR_PRIM *sprm_pp, TIM2_DAT *tmd_pp) {
    int timer;

    sprm_pp->w = tmd_pp->w;
    sprm_pp->h = tmd_pp->h;
    FadeCtrlReq(FMODE_BLACK_IN, 30);

    timer = 29;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }

    timer = 119;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }

    FadeCtrlReq(FMODE_BLACK_OUT, 30);

    timer = 29;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }
}

INCLUDE_RODATA("main/main", D_00393A00);
INCLUDE_RODATA("main/main", D_00393A18);

// From SpHatChangeSub
INCLUDE_RODATA("main/main", D_00393A38);
INCLUDE_RODATA("main/main", D_00393A48);
INCLUDE_RODATA("main/main", D_00393A58);
INCLUDE_RODATA("main/main", D_00393A68);

static void uramenFileSearchTask(void *x) {
    printf("file search in\n");
    stDatFirstFileSearch();
    printf("file search out\n");

    uramen_end_flag = 0;
    MtcExit();
}

static void uramenFileSearchSet(void) {
    uramen_end_flag = 1;
    MtcExec(uramenFileSearchTask, MTC_TASK_03);
}

static void uramenFileSearchEnd(void) {
    if (!uramen_end_flag) {
        return;
    }

    while (uramen_end_flag) {
        MtcWait(1);
    }
}

void startUpDisp(void) {
    /* Splash screens TIM2 data */
    /* TODO: match .data (see symbol_addrs.txt) */
    extern TIM2_DAT tim2spr_tbl_tmp0[2]; /* =
    {
        { .GsTex0 = 0x20050A022531A800, .GsTex1 = 0x260, .w = 320, .h = 224 }, / NanaOn-Sha /
        { .GsTex0 = 0x20150A81A5422968, .GsTex1 = 0x260, .w = 440, .h = 52  }  / SCEI presents /
    }; */

    /* Splash screens sprite data */
    SPR_PRIM spr_prim[2] = {
        { .x = 2048, .y = 2048, .scalex = 256, .scaley = 128 },
        { .x = 2048, .y = 2048, .scalex = 512, .scaley = 256 }
    };

    UsrMemClear();
    SpuBankSet();

    CdctrlRead(&file_str_logo_file, UsrMemAllocNext(), 0);
    CdctrlReadWait();

    SprInit();
    MenuMemCardCheck();

    uramenFileSearchSet();
    logoDispOne(&spr_prim[0], &tim2spr_tbl_tmp0[1]);
    logoDispOne(&spr_prim[1], &tim2spr_tbl_tmp0[0]);
    uramenFileSearchEnd();

    UsrMemClear();
    SpuBankSet();
}

INCLUDE_ASM("main/main", selPlayDispType);

INCLUDE_ASM("main/main", selPlayDispSetPlay);

INCLUDE_ASM("main/main", selPlayDispSetPlayOne);

INCLUDE_ASM("main/main", gamePlayDisp);
int gamePlayDisp(void);

#ifndef NON_MATCHING
INCLUDE_ASM("main/main", titleDisp);
#else
void titleDisp(/* s1 17 */ int firstf)
{
    /* s0 16 */ STDAT_DAT *stdat_dat_pp;
    /* v0 2 */ int fsize;
    /* s0 16 */ int decp;
    /* s2 18 */ int loop;
    /* s3 19 */ int deramode;

    deramode = 0;
    loop = 0;

    while (1)
    {
        UsrMemClear();
        SpuBankSet();

        game_status.demo_flagG = DEMOF_OFF;
        GlobalLobcalCopy();

        stdat_dat_pp = stdat_rec[19].stdat_dat_pp;
        fsize = (CdctrlGetFileSize(&stdat_dat_pp->intfile) + 2047) / 2048;
        decp = UsrMemEndAlloc(fsize / 2048);

        CdctrlReadOne(&stdat_dat_pp->intfile, decp, 0);
        CdctrlReadWait();

        if (loop == 0)
        {
            selPlayDisp(0, 0, firstf);
            WipeInReqSame();
            SetBackColor(255, 255, 255);
        }

        MtcWait(2);

        UsrMemClearTop();
        UsrMemEndFree();
        CdctrlMemIntgDecode(UsrMemAllocEndNext(), UsrMemAllocNext());

        game_status.demo_flagG = DEMOF_OFF;
        GlobalLobcalCopy();

        if (selPlayDispTitleDisp(19, deramode, loop))
        {
            SetBackColor(0, 0, 0);
            break;
        }

        firstf = 0;

        SetBackColor(0, 0, 0);
        deramode ^= 1;

        WipeInReq();
        MtcWait(2);

        SpuBankSet();
        UsrMemClear();

        game_status.demo_flagG = DEMOF_DEMO;
        loop = gamePlayDisp();

        WipeInReq();
        MtcWait(2);
    }
}
#endif

INCLUDE_RODATA("main/main", D_00393AD0);

INCLUDE_ASM("main/main", urawazaKeyCheck);
int urawazaKeyCheck(void);

extern char D_003996D0[]; /* sdata - "ura:%d" */

void ura_check(void) {
    u_char msg_tmp[32];
    int    ret;

    DbgMsgInit();

    while (1) {
        MtcWait(1);

        ret = urawazaKeyCheck();
        if (ret < 0) {
            sprintf(msg_tmp, "ura:NOUSE");
        } else {
            sprintf(msg_tmp, D_003996D0, ret);
        }

        DbgMsgClear();

        DbgMsgSetSize(22, 10);
        DbgMsgSetColor(128, 128, 128);
        DbgMsgPrint(msg_tmp, 1820, 1948);

        DbgMsgFlash();
    }
}

void mainStart(void *xx) {
    /* sdata 3996d8 */ extern int first_f;
    int retTitle;

    mccReqInit();
    CdctrlInit();

    PrInitializeModule(DBufDc.draw01.zbuf1);
    UsrPrInitScene();

    hat_change_enum = HCNG_AUTO;

    TapCtInit();
    TapCt(0, 0, 0);

    TimeCallbackSet();
    GlobalInit();
    UsrMemClear();
    SpuBankSetAll();

    menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
    menu_str.mc_rep_str_p = &mc_rep_str;
    menu_str.game_status_p = &game_status;

    cmnfTim2Trans();
    wipeSndFileTrans();

    CdctrlReadWait();
    printf("int read end\n");

    startUpDisp();

    while (1) {
        urawaza_levelsel_bottun = -1;
        titleDisp(first_f);

        WipeInReq();
        MtcWait(2);

        first_f = 0;

        menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
        game_status.demo_flagG = DEMOF_OFF;

        while (1) {
            UsrMemClear();
            SpuBankSet();

            urawaza_levelsel_bottun = -1;
            urawaza_skip_bottun = 0;

            CdctrlRead(&file_str_menu_file, UsrMemAllocNext(), UsrMemAllocEndNext());
            CdctrlReadWait();

            while (!WipeEndCheck()) {
                MtcWait(1);
            }
            WipeOutReq();

            PrSetStage(0);
            game_status.play_typeG = PLAY_TYPE_NORMAL;

            retTitle = MenuCtrl(&menu_str);

            /*
             * Cheat code: L1 + R1
             *
             * Skip cutscenes + Boxy Boy practice.
             */
            if (pad[0].shot & SCE_PADL1 &&
                pad[0].shot & SCE_PADL2) {
                urawaza_skip_bottun = 1;
            } else {
                urawaza_skip_bottun = 0;
            }

            /*
             * Cheat code: L2 + R2
             *
             * Enable 'shuriken' mode.
             */
            if (pad[0].shot & SCE_PADR2 &&
                pad[0].shot & SCE_PADR1 &&
                game_status.play_modeG == PLAY_MODE_SINGLE) {
                game_status.play_typeG = PLAY_TYPE_ONE;
            } else {
                game_status.play_typeG = PLAY_TYPE_NORMAL;
            }

            /*
             * Cheat code: R3 + Right analog stick
             *
             * Change stage difficulty, can only be
             * used given the following conditions:
             *    - Singleplayer mode.
             *    - 'Shuriken' mode is disabled.
             *    - Max round/circuit (Yellow hat).
             *    - Current stage is not a demo.
             *
             *    (TODO): Document:
             *      - game_status.play_table_modeG != PLAY_TABLE_EASY
             */
            if (game_status.play_modeG == PLAY_MODE_SINGLE &&
                game_status.play_typeG == PLAY_TYPE_NORMAL &&
                game_status.roundG >= TRND_R4 &&
                game_status.demo_flagG == DEMOF_OFF &&
                game_status.play_table_modeG != PLAY_TABLE_EASY) {
                urawaza_levelsel_bottun = urawazaKeyCheck();
            } else {
                urawaza_levelsel_bottun = -1;
            }

            if (game_status.play_typeG == PLAY_TYPE_ONE) {
                if (game_status.endingFlag == 2) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 3) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 4) {
                    game_status.endingFlag = 0;
                }
            }

            if (game_status.play_table_modeG == PLAY_TABLE_EASY) {
                if (game_status.endingFlag == 2) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 3) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 4) {
                    game_status.endingFlag = 0;
                }
            }

            if (dbg_select_str.debug_on && !retTitle) {
                dbg_select_disp();
            }

            WipeInReq();
            UsrMemClear();
            SpuBankSet();

            if (retTitle) {
                break;
            }

            if (dbg_select_str.debug_on && dbg_select_str.non_play) {
                while (!WipeEndCheck()) {
                    MtcWait(1);
                }
                WipeOutReq();

                dummyPlay(retTitle);
            } else {
                gamePlayDisp();
            }

            WipeInReq();
            MtcWait(2);

            urawaza_levelsel_bottun = -1;
        }
    }
}
