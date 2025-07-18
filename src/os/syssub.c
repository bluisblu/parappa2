#include "os/syssub.h"

#include "os/system.h"

#include <libpad.h>

#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PAD_SYSD sysPad[2] PR_ALIGNED(64) = {};

static u_long128 pad_dma_buf[2][16];
static u_long128 ChangeDrawAreaPacket[12];
static sceDmaTag exl_dmatag;
static USR_MALLOC_STR usr_malloc_str[256];

#define PAD_NUM (2)

void WorkClear(void *clr_adrs, int size) {
    u_char *clr_pp = (u_char*)clr_adrs;

    while (size--) {
        *clr_pp++ = 0;
    }
}

void GPadInit(void) {
    int i;

    WorkClear(pad, sizeof(pad));
    WorkClear(sysPad, sizeof(sysPad));

    scePadInit(0);

    for (i = 0; i < PAD_NUM; i++) {
        if (!scePadPortOpen(i, 0, pad_dma_buf[i])) {
            printf("ERROR: scePadPortOpen[%d]\n", i);
        }
    }
}

void GPadExit(void) {
    int i;

    for (i = 0; i < PAD_NUM; i++) {
        scePadPortClose(i, 0);
    }

    scePadEnd();
}

void GPadSysRead(void) {
    int       i;
    int       state;
    int       id, exid;
    PAD_SYSD *sysP_pp;

    sysP_pp = sysPad;

    for (i = 0; i < PAD_NUM; i++) {
        state = scePadGetState(i, 0);
        WorkClear(sysP_pp->rdata, 32);

        switch (state) {
        case scePadStateFindCTP1:
        case scePadStateStable:
            switch (sysP_pp->phase) {
            case 0: /* Initial phase */
                id = scePadInfoMode(i, 0, InfoModeCurID, 0);
                exid = scePadInfoMode(i, 0, InfoModeCurExID, 0);

                if (exid > 0) {
                    id = exid;
                }

                if (id == 0) {
                    WorkClear(sysP_pp, sizeof(*sysP_pp));
                } else {
                    sysP_pp->pad_id = id;

                    switch (id) {
                    case 4: /* Standard */
                        sysP_pp->phase = 40;
                        break;
                    case 7: /* Analog */
                        sysP_pp->phase = 70;
                        break;
                    default:
                        WorkClear(sysP_pp, sizeof(*sysP_pp));
                        break;
                    }
                }

                break;
            case 40: /* Initial standard pad phase */
                if (scePadInfoMode(i, 0, InfoModeIdTable, -1) == 0) {
                    /* Go to read phase if no modes */
                    sysP_pp->phase = 99;
                    break;
                }
                sysP_pp->phase++;
            case 41: /* Standard mode switch phase */
                /* Try to switch to analog(?) mode */
                if (scePadSetMainMode(i, 0, 1, 3) == 1) {
                    sysP_pp->phase++;
                }
                break;
            case 42:
                if (scePadGetReqState(i, 0) == scePadReqStateFaild) {
                    /* Switch failed, go back and keep trying. */
                    sysP_pp->phase--;
                }
                if (scePadGetReqState(i, 0) == scePadReqStateComplete) {
                    /*
                     * Switch successful, go back to the initial phase
                     * to setup analog mode.
                     */
                    sysP_pp->phase = 0;
                }
                break;
            case 70: /* Initial analog phase */
                /* Get the number of actuators */
                if (!scePadInfoAct(i, 0, -1, 0)) {
                    sysP_pp->phase = 99;
                    break;
                }
                /* Try to switch to analog(?) mode */
                if (scePadSetMainMode(i, 0, 1, 3) == 1) {
                    sysP_pp->phase++;
                    break;
                }
                break;
            case 72:
                if (scePadInfoPressMode(i, 0) == 1) {
                    sysP_pp->phase = 76;
                } else {
                    sysP_pp->phase = 80;
                }
                break;
            case 76:
                if (scePadEnterPressMode(i, 0) == 1) {
                    sysP_pp->phase++;
                }
                break;
            case 71:
            case 77:
                if (scePadGetReqState(i, 0) == scePadReqStateFaild) {
                    /* Switch failed, go back and keep trying. */
                    sysP_pp->phase--;
                }
                if (scePadGetReqState(i, 0) == scePadReqStateComplete) {
                    /* Switch succesful, go to actuator setup phase */
                    sysP_pp->phase = 80;
                }
                break;
            case 80: /* Analog actuator setup phase */
                /* Get the number of actuators */
                if (scePadInfoAct(i, 0, -1, 0) == 0) {
                    sysP_pp->phase = 99;
                }

                /*
                 * Initialize actuator parameters:
                 *
                 * act_align[0] = Actuator 0
                 * act_align[1] = Actuator 1
                 * act_align[2...5] = Unused (0xff)
                 */
                sysP_pp->act_align[0] = 0x0;
                sysP_pp->act_align[1] = 0x1;
                sysP_pp->act_align[2] = 0xff;
                sysP_pp->act_align[3] = 0xff;
                sysP_pp->act_align[4] = 0xff;
                sysP_pp->act_align[5] = 0xff;
                WorkClear(sysP_pp->act_direct, 6);

                if (scePadSetActAlign(i, 0, sysP_pp->act_align) == 0) {
                    break;
                }
                
                sysP_pp->phase++;
                break;
            case 81:
                if (scePadGetReqState(i, 0) == scePadReqStateFaild) {
                    /* scePadSetActAlign failed. Try again. */
                    sysP_pp->phase--;
                }
                if (scePadGetReqState(i, 0) == scePadReqStateComplete) {
                    /* scePadSetActAlign succeeded, go to read phase. */
                    sysP_pp->phase = 99;
                }
                break;
            default: /* Read phase (default/99) */
                if (scePadRead(i, 0, (u_char*)&sysP_pp->rdata) == 0 ||
                    scePadInfoAct(i, 0, -1, 0) == 0) {
                    break;
                }
                scePadSetActDirect(i, 0, sysP_pp->act_direct);
                break;
            }
            break;
        case scePadStateDiscon:
            WorkClear(sysP_pp, sizeof(*sysP_pp));
            break;
        default:
            break;
        }

        sysP_pp++;
    }
}

void padMakeData(PADD *pad_pp, u_short paddata) {
    pad_pp->old = pad_pp->shot;
    pad_pp->shot = paddata;

    /*
     * pad_pp->one: Buttons just pushed on this frame.
     * pad_pp->off: Buttons just released on this frame.
     */
    pad_pp->one = (pad_pp->old ^ pad_pp->shot) & pad_pp->shot;
    pad_pp->off = (pad_pp->old ^ pad_pp->shot) & ~pad_pp->shot;
}

void pad0Clear(PADD *pad_pp) {
    padMakeData(pad_pp, 0);
}

void padOneOffBitCLear(PADD *pad_pp) {
    pad_pp->one = 0;
    pad_pp->off = 0;
}

void padNormalRead(PADD *pad_pp, u_char *rdata_pp) {
    /*
     * Join button state into a short and swap
     * the state (prev. it was [0: pushed, 1: released]).
     */
    padMakeData(pad_pp, ~((rdata_pp[2] << 8) | rdata_pp[3]));
}

void padAnaRead(PADD *pad_pp, u_char *rdata_pp) {
    int     i;
    u_char *ana_pp = pad_pp->ana;

    /* Advance buffer into analog stick data. */
    rdata_pp += 4;

    for (i = 0; i < PR_ARRAYSIZE(pad_pp->ana); i++) {
        ana_pp[i] = rdata_pp[i];
    }
}

void padAnaRead0Clear(PADD *pad_pp) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(pad_pp->ana); i++) {
        pad_pp->ana[i] = 0x80;
    }
}

void padPrsRead(PADD *pad_pp, u_char *rdata_pp) {
    int     i;
    u_char *prs_pp = pad_pp->press;

    /* Advance buffer into pressure sensitivity data. */
    rdata_pp += 8;

    for (i = 0; i < PR_ARRAYSIZE(pad_pp->press); i++) {
        prs_pp[i] = rdata_pp[i];
    }
}

void padPrsRead0Clear(PADD *pad_pp) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(pad_pp->press); i++) {
        pad_pp->press[i] = 0;
    }
}

void padPrsTreate(PADD *pad_pp) {
    u_short pad_pos[12] = {
        /* D-pad buttons */
        SCE_PADLright, SCE_PADLleft,
        SCE_PADLup,    SCE_PADLdown,

        /* Face buttons */
        SCE_PADRup,    SCE_PADRright,
        SCE_PADRdown,  SCE_PADRleft,

        /* Trigger buttons */
        SCE_PADL1,     SCE_PADR1,
        SCE_PADL2,     SCE_PADR2,
    };

    int i;
    for (i = 0; i < PR_ARRAYSIZEU(pad_pos); i++) {
        if (pad_pp->shot & pad_pos[i]) {
            if (!pad_pp->press[i]) {
                pad_pp->press[i] = 1;
            }
        } else {
            if (pad_pp->press[i]) {
                pad_pp->press[i] = 0;
            }
        }
    }
}

void padActSet(PADD *pad_pp, PAD_SYSD *sysPad_pp) {
    if (pad_pp != NULL) {
        /* Set vibration info. */
        sysPad_pp->act_direct[0] = pad_pp->padvib[0];
        sysPad_pp->act_direct[1] = pad_pp->padvib[1];
    } else {
        sysPad_pp->act_direct[0] = 0;
        sysPad_pp->act_direct[1] = 0;
    }
}

void padActClear(PADD *pad_pp) {
    pad_pp->padvib[0] = 0;
    pad_pp->padvib[1] = 0;
}

void padAnaMixPad(PADD *pad_pp) {
    u_short old_shot = pad_pp->mshot;

    pad_pp->mshot = pad_pp->shot;

    /* Analog X axis */
    if (pad_pp->ana[PAD_ANA_LX] < 0x40) {
        pad_pp->mshot |= SCE_PADLleft;
    }
    if (pad_pp->ana[PAD_ANA_LX] > 0xbf) {
        pad_pp->mshot |= SCE_PADLright;
    }

    /* Analog Y axis */
    if (pad_pp->ana[PAD_ANA_LY] < 0x40) {
        pad_pp->mshot |= SCE_PADLup;
    }
    if (pad_pp->ana[PAD_ANA_LY] > 0xbf) {
        pad_pp->mshot |= SCE_PADLdown;
    }

    pad_pp->mone = pad_pp->mshot & (old_shot ^ pad_pp->mshot);
}

void GPadRead(PADD *pad_pp) {
    int i;

    for (i = 0; i < PAD_NUM; i++, pad_pp++) {
        if (sysPad[i].rdata[0] != 0) {
            /* Pad connection wasn't successful. */
            pad0Clear(pad_pp);
        }

        switch (sysPad[i].rdata[1] & 0xf0) {
        case PAD_ENUM_DSHOCK: /* DualShock controller */
            padNormalRead(pad_pp, sysPad[i].rdata);
            padAnaRead(pad_pp, sysPad[i].rdata);

            /*
             * The DualShock2 has the same ID as the original
             * DualShock, but we can detect for a DSHOCK2 by
             * checking the data length instead (lower 4 bits).
             *
             * This check is hidden away in the enum:
             * - PAD_ENUM_DSHOCK2 = 0111   1001
             *                     \____/ \____/
             *                  ID (DSHOCK)
             *                          Data length
             *
             * If the current controller is a DSHOCK2, the
             * data length will be 9*2 = 18 bytes (ignoring
             * the first two bytes).
             */
            if (sysPad[i].rdata[1] == PAD_ENUM_DSHOCK2) {
                padPrsRead(pad_pp, sysPad[i].rdata);
                pad_pp->padId = PAD_ENUM_DSHOCK2;
                padPrsTreate(pad_pp);
            } else {
                padPrsRead0Clear(pad_pp);
                pad_pp->padId = PAD_ENUM_DSHOCK;
            }

            padActSet(pad_pp, &sysPad[i]);
            break;
        case PAD_ENUM_NORMAL: /* Digital controller */
            padNormalRead(pad_pp, sysPad[i].rdata);
            padAnaRead0Clear(pad_pp);
            padPrsRead0Clear(pad_pp);

            pad_pp->padId = PAD_ENUM_NORMAL;
            padActSet(NULL, &sysPad[i]);
            break;
        default: /* Unknown/unsupported controller. */
            pad0Clear(pad_pp);
            padAnaRead0Clear(pad_pp);
            padPrsRead0Clear(pad_pp);
            break;
        }

        padAnaMixPad(pad_pp);
        padActClear(pad_pp);
    }
}

PAD_PRESS_ENUM GetPadbit2PressId(u_short padbit) {
    BIT2PR bit2pr[12] = {
        { 0x8000, PAD_PR_Lleft }, { 0x2000, PAD_PR_Lright },
        { 0x1000, PAD_PR_Lup   }, { 0x4000, PAD_PR_Ldown  },

        { 0x10,   PAD_PR_Rup   }, { 0x40,   PAD_PR_Rdown  },
        { 0x80,   PAD_PR_Rleft }, { 0x20,   PAD_PR_Rright },

        { 0x4,    PAD_PR_L1    }, { 0x1,    PAD_PR_L2     },
        { 0x8,    PAD_PR_R1    }, { 0x2,    PAD_PR_R2     }
    };
    PAD_PRESS_ENUM ret = PAD_PR_None;
    u_int i;

    for (i = 0; i < 12; i++) {
        if ((bit2pr[i].bit & padbit) != 0) {
            ret = bit2pr[i].prn;
            break;
        }
    }

    return ret;
}

u_char GetPadbit2PressPad(PADD *pad_pp, u_short padbit) {
    if (GetPadbit2PressId(padbit) == PAD_PR_None) {
        return 0;
    }

    return pad_pp->press[padbit];
}

void SetBackColor(u_char R, u_char G, u_char B) {
    DBufDc.clear0.rgbaq.R = R;
    DBufDc.clear0.rgbaq.G = G;
    DBufDc.clear0.rgbaq.B = B;
    DBufDc.clear1.rgbaq = DBufDc.clear0.rgbaq;
}

void ChangeDrawArea(sceGsDrawEnv1 *env_pp) {
    u_long giftag[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), SCE_GIF_PACKED_AD };
    sceGifPacket gifpk;
    sceDmaChan *cmnDmaC;

    sceGifPkInit(&gifpk, ChangeDrawAreaPacket);
    sceGifPkReset(&gifpk);
    sceGifPkCnt(&gifpk, 0, 0, 0);

    sceGifPkOpenGifTag(&gifpk, *(u_long128*)giftag);
    sceGifPkAddGsAD(&gifpk, SCE_GS_FRAME_1, *(u_long*)&env_pp->frame1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_FRAME_2, *(u_long*)&env_pp->frame1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_XYOFFSET_1, *(u_long*)&env_pp->xyoffset1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_XYOFFSET_2, *(u_long*)&env_pp->xyoffset1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_SCISSOR_1, *(u_long*)&env_pp->scissor1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_SCISSOR_2, *(u_long*)&env_pp->scissor1);
    sceGifPkCloseGifTag(&gifpk);

    sceGifPkEnd(&gifpk, 0, 0, 0);
    sceGifPkTerminate(&gifpk);

    cmnDmaC = sceDmaGetChan(SCE_DMA_GIF);

    FlushCache(WRITEBACK_DCACHE);

    sceDmaSend(cmnDmaC, gifpk.pBase);
    sceGsSyncPath(0,0);
}

void ChangeDrawAreaSetGifTag(sceGsDrawEnv1 *env_pp, sceGifPacket *gifpk_pp) {
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_FRAME_1, *(u_long*)&env_pp->frame1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_FRAME_2, *(u_long*)&env_pp->frame1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYOFFSET_1, *(u_long*)&env_pp->xyoffset1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYOFFSET_2, *(u_long*)&env_pp->xyoffset1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_SCISSOR_1, *(u_long*)&env_pp->scissor1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_SCISSOR_2, *(u_long*)&env_pp->scissor1);
}

void ChangeDrawArea2(sceGsDrawEnv1 *env_pp) {
    u_long giftag[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), SCE_GIF_PACKED_AD };
    sceGifPacket gifpk;
    sceDmaChan *cmnDmaC;

    sceGifPkInit(&gifpk, ChangeDrawAreaPacket);
    sceGifPkReset(&gifpk);
    sceGifPkCnt(&gifpk, 0, 0, 0);

    sceGifPkOpenGifTag(&gifpk, *(u_long128*)giftag);
    sceGifPkAddGsAD(&gifpk, SCE_GS_FRAME_2, *(u_long*)&env_pp->frame1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_XYOFFSET_2, *(u_long*)&env_pp->xyoffset1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_SCISSOR_2, *(u_long*)&env_pp->scissor1);
    sceGifPkCloseGifTag(&gifpk);

    sceGifPkEnd(&gifpk, 0, 0, 0);
    sceGifPkTerminate(&gifpk);

    cmnDmaC = sceDmaGetChan(SCE_DMA_GIF);

    FlushCache(WRITEBACK_DCACHE);

    sceDmaSend(cmnDmaC, gifpk.pBase);
    sceGsSyncPath(0,0);
}

void ClearFrameBufferGifTag(sceGsFrame *draw_pp, sceGifPacket *gifpk_pp, u_char r, u_char g, u_char b, u_char a) {
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_FRAME_1, *(u_long*)draw_pp);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 0, 0, 1, 0, 0, 1, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(r, g, b, a, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(0, 640, 0, 224));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYOFFSET_1, SCE_GS_SET_XYOFFSET(27648, 30976)); // ???
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_PRIM_SPRITE);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ(27648, 30976, 1)); // ???
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ(37888, 34560, 1)); // ???
}

void GGsSetLocalMoveImage(
    sceGsMoveImage *lp, short dbp, short dbw, short dpsm,
    short dx, short dy, short sbp, short sbw,
    short spsm, short sx, short sy, short w, short h, short dir) {
    lp->giftag.NLOOP = 4;
    lp->giftag.EOP = 1;
    lp->giftag.FLG = 0;
    lp->giftag.NREG = 1;
    lp->giftag.REGS0 = SCE_GIF_PACKED_AD;
    
    lp->bitbltbuf.SBP = sbp;
    lp->bitbltbuf.SBW = sbw;
    lp->bitbltbuf.SPSM = spsm;
    lp->bitbltbuf.DBP = dbp;
    lp->bitbltbuf.DBW = dbw;
    lp->bitbltbuf.DPSM = dpsm;
    lp->bitbltbufaddr = SCE_GS_BITBLTBUF;
    
    lp->trxpos.SSAX = sx;
    lp->trxpos.SSAY = sy;
    lp->trxpos.DSAX = dx;
    lp->trxpos.DSAY = dy;
    lp->trxpos.DIR = dir;
    lp->trxposaddr = SCE_GS_TRXPOS;
    
    lp->trxreg.RRW = w;
    lp->trxreg.RRH = h;
    lp->trxregaddr = SCE_GS_TRXREG;
    
    lp->trxdir.XDR = 2;
    lp->trxdiraddr = SCE_GS_TRXDIR;
}

void GGsExecLocalMoveImage(sceGsMoveImage *lp) {
    exl_dmatag.qwc = 5;
    exl_dmatag.id = 0;
    exl_dmatag.next = (sceDmaTag*)lp;

    sceDmaSync(sceDmaGetChan(SCE_DMA_GIF), 0, INT_MAX);
    FlushCache(WRITEBACK_DCACHE);
    sceDmaSend(sceDmaGetChan(SCE_DMA_GIF), &exl_dmatag);
}

u_int randMakeMax(u_int max) {
    u_int ret = (rand() & 0x7fff) * max;
    return ret >> 15;
}

static char* ByteStringSub(char *s, unsigned int n) {
    if (n >= 1000) {
        s = ByteStringSub(s, n / 1000);
        *s++ = ',';
        return s + sprintf(s, "%03u", n % 1000);
    } else {
        return s + sprintf(s, "%u", n % 1000);
    }
}

char* ByteString(unsigned int n) {
    static char s[50];
    memcpy(ByteStringSub(s, n), "(byte)", 7);
    return s;
}

u_int ReportHeapUsage(void) {
    struct mallinfo malloc_info = mallinfo();

    printf("_________________________________________\n");
    printf("total space allocated from system = %s\n", ByteString(malloc_info.arena));
    printf("total space in mmapped regions = %s\n", ByteString(malloc_info.hblkhd));
    printf("total allocated space = %s\n", ByteString(malloc_info.uordblks));
    printf("total non-inuse space = %s\n", ByteString(malloc_info.fordblks));
    printf("number of non-inuse chunks = %d\n", malloc_info.ordblks);
    printf("number of mmapped regions = %d\n", malloc_info.hblks);
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

    return malloc_info.uordblks;
}

void usrMallcInit(void) {
    WorkClear(usr_malloc_str, sizeof(usr_malloc_str));
}

void* usrMalloc(u_int size) {
    void *ret;
    int i;

    ret = malloc(size);
    if (ret == 0) {
        printf("malloc is NG\n");
        return 0;
    }

    for (i = 0; i < PR_ARRAYSIZE(usr_malloc_str); i++) {
        if (usr_malloc_str[i].adrs == 0) {
            usr_malloc_str[i].adrs = ret;
            usr_malloc_str[i].size = size;
            return ret;
        }
    }

    free(ret);
    printf("malloc is NG\n");
    return 0;
}

void usrFree(void *adrs) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(usr_malloc_str); i++) {
        if (usr_malloc_str[i].adrs != NULL &&
            usr_malloc_str[i].adrs == adrs) {
            free(adrs);
            usr_malloc_str[i].adrs = 0;
            usr_malloc_str[i].size = 0;
            return;
        }
    }

    printf("free is NG\n");
}

void usrMallcReport(void) {
    int i;
    int cnt = 0;

    printf("--- usr malloc report ---\n");

    for (i = 0; i < PR_ARRAYSIZE(usr_malloc_str); i++) {
        if (usr_malloc_str[i].adrs) {
            printf(" use  adr[%d] size[%d]\n", usr_malloc_str[i].adrs, usr_malloc_str[i].size);
            cnt++;
        }
    }

    printf("--- use cnt[%d] ---\n", cnt);
}

sceGsDrawEnv1* DrawGetDrawEnvP(DNUM_ENUM dnum) {
    int ret;

    switch (dnum) {
    case DNUM_SHOW:
        ret = (outbuf_idx != 0);
        break;
    case DNUM_DRAW:
        ret = (outbuf_idx == 0);
        break;
    case DNUM_VRAM0:
        ret = 0;
        break;
    case DNUM_VRAM1:
        ret = 1;
        break;
    case DNUM_VRAM2:
        ret = 2;
        break;
    case DNUM_ZBUFF:
        ret = 3;
        break;
    case DNUM_END:
        ret = 4;
        break;
    default:
        return NULL;
    }
    
    return drawEnvP[ret];
}

sceGsFrame* DrawGetFrameP(DNUM_ENUM dnum) {
    int ret;

    switch (dnum) {
    case DNUM_SHOW:
        ret = (outbuf_idx != 0);
        break;
    case DNUM_DRAW:
        ret = (outbuf_idx == 0);
        break;
    case DNUM_VRAM0:
        ret = 0;
        break;
    case DNUM_VRAM1:
        ret = 1;
        break;
    case DNUM_VRAM2:
        ret = 2;
        break;
    case DNUM_ZBUFF:
        ret = 3;
        break;
    case DNUM_END:
        ret = 4;
        break;
    default:
        return NULL;
    }

    return &drawEnvP[ret]->frame1;
}

int DrawGetFbpPos(DNUM_ENUM dnum) {
    int ret = 0;

    // TODO(poly): figure out what these values mean
    switch (dnum)  {
    case DNUM_SHOW:
        ret = 70;
        if (outbuf_idx == 0) {
            ret = 0;
        }
        break;
    case DNUM_DRAW:
        ret = 70;
        if (outbuf_idx != 0) {
            ret = 0;
        }
        break;
    case DNUM_VRAM0:
        ret = 0;
        break;
    case DNUM_VRAM1:
        ret = 70;
        break;
    case DNUM_VRAM2:
        ret = 210;
        break;
    case DNUM_ZBUFF:
        ret = 140;
        break;
    case DNUM_END:
        ret = 320;
        break;
    }

    return ret;
}

int DrawGetTbpPos(DNUM_ENUM dnum) {
    int ret = 0;

    // TODO(poly): figure out what these values mean
    switch (dnum) {
    case DNUM_SHOW:
        ret = 2240;
        if (outbuf_idx == 0) {
            ret = 0;
        }
        break;
    case DNUM_DRAW:
        ret = 2240;
        if (outbuf_idx != 0) {
            ret = 0;
        }
        break;
    case DNUM_VRAM0:
        ret = 0;
        break;
    case DNUM_VRAM1:
        ret = 2240;
        break;
    case DNUM_VRAM2:
        ret = 6720;
        break;
    case DNUM_ZBUFF:
        ret = 4480;
        break;
    case DNUM_END:
        ret = 10240;
        break;
    }

    return ret;
}
