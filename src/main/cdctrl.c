#include "main/cdctrl.h"

#include "os/mtc.h"
#include "os/syssub.h"
#include "os/tim2.h"
#include "os/usrmem.h"

#include "main/etc.h"
#include "main/p3str.h"

#include "iop_mdl/tapctrl_rpc.h"
#include "iop_mdl/wp2cd_rpc.h"

#include <eeregs.h>
#include <sifdev.h>
#include <stdio.h>

/* sdata */
void *current_intg_adrs = NULL;

/* bss - static */
extern CDCTRL_STR cdctrl_str;

/* sbss - static */
static int cdSampleTmp;

#define N        4096 /* Size of ring buffer */
#define F          18 /* Upper limit */
#define THRESHOLD   2

/* bss - static */
extern unsigned char RBuff[N + F - 1]; /* Ring buffer for INT decompression */

#define lzss_read()   *(fp_r)++;
#define lzss_write(x) *(fp_w)++ = (x);

#define PACK(x) ((PACKINT_FILE_STR*)x)

static int cdctrlReadSub(FILE_STR *fstr_pp, int ofs, int size, int buf);
static void cdctrlReadDataOne(void *x);
static void cdctrlSndFadeOut(void *x);

u_int PackIntGetDecodeSize(u_char *fp_r) {
    return *(u_int*)fp_r;
}

int PackIntDecode(u_char *fp_r, u_char *fp_w) {
    u_int        moto_size;     /* Decode size                                  */
    int          rp    = N - F; /* Ring buffer position                         */

    unsigned int flags = 0;     /* LZSS flags                                   */
    u_char      *fp_w_end;      /* End of write buffer (buffer + decode size)   */

    int          i, c, c1, c2;
    
    asm("sync.l");

    moto_size = *(u_int*)fp_r;
    fp_w = (u_char*)PR_UNCACHED(fp_w);
    fp_w_end = fp_w + moto_size;

    fp_r += 8;

    for (i = 0; i < N - F; i++) {
        RBuff[i] = 0;
    }

    while (fp_w != fp_w_end) {
        if (((flags >>= 1) & 256) == 0) {
            c = lzss_read();
            flags = c | 0xff00;
        }

        if (flags & 1) {
            c = lzss_read();
            lzss_write(c);

            RBuff[rp++] = c;
            rp &= (N - 1);
        } else {
            c1 = lzss_read();
            c2 = lzss_read();

            c1 |= ((c2 & 0xf0) << 4);
            c2 = (c2 & 0x0f) + THRESHOLD;

            for (i = 0; i <= c2; i++) {
                c = RBuff[(c1 + i) & (N - 1)];
                lzss_write(c);
                
                RBuff[rp++] = c;
                rp &= (N - 1);
            }
        }
    }

    asm("sync.l");
    return 0;
}

int PackIntDecodeWait(u_char *fp_r, u_char *fp_w, int wait_hline) {
    u_int        moto_size;     /* Decode size                                  */
    u_char      *fp_w_end;      /* End of write buffer (buffer + decode size)   */

    int          rp    = N - F; /* Ring buffer position                         */
    unsigned int flags = 0;     /* LZSS flags                                   */

    int          i, c, c1, c2;
    
    printf("decode moto[%08x] saki[%08x]\n", fp_r, fp_w);
    asm("sync.l");

    moto_size = *(u_int*)fp_r;
    fp_w = (u_char*)PR_UNCACHED(fp_w);
    fp_w_end = fp_w + moto_size;

    fp_r += 8;

    for (i = 0; i < N - F; i++) {
        RBuff[i] = 0;
    }

    while (1) {
        if (fp_w > fp_w_end) {
            printf(" over data pointer\n");
            break;
        }
        if (fp_w == fp_w_end) {
            break;
        }

        if (wait_hline < *T0_COUNT) {
            MtcWait(1);
        }

        if (((flags >>= 1) & 256) == 0) {
            c = lzss_read();
            flags = c | 0xff00;
        }

        if (flags & 1) {
            c = lzss_read();
            lzss_write(c);

            RBuff[rp++] = c;
            rp &= (N - 1);
        } else {
            c1 = lzss_read();
            c2 = lzss_read();

            c1 |= ((c2 & 0xf0) << 4);
            c2 = (c2 & 0x0f) + THRESHOLD;

            for (i = 0; i <= c2; i++) {
                c = RBuff[(c1 + i) & (N - 1)];
                lzss_write(c);
                
                RBuff[rp++] = c;
                rp &= (N - 1);
            }
        }
    }

    asm("sync.l");
    return 0;
}

void CdctrlInit(void) {
    WorkClear(&cdctrl_str, sizeof(cdctrl_str));

    printf("in wp2 init\n");
    WP2Init();
    printf("out wp2 init\n");

    WP2Ctrl(WP2_CDINIT, SCECdDVD);
    WP2Ctrl(WP2_SDINIT, WP2_NONE);
    WP2Ctrl(WP2_SETMASTERVOL, PR_CONCAT(0x3fff, 0x3fff));

    /*
     * WP2 init--allocate 384KB for the read
     * buffer in 768 blocks of 512 bytes each.
     */
    WP2Ctrl(WP2_INIT, 0x300);
}

void CdctrlQuit(void) {
    WorkClear(&cdctrl_str, sizeof(cdctrl_str));
    WP2Ctrl(WP2_QUIT, WP2_NONE);
}

void CdctrlMasterVolSet(u_int vol) {
    WP2Ctrl(WP2_SETMASTERVOL, PR_CONCAT(vol, vol));
}

int CdctrlSerch(FILE_STR *fstr_pp) {
    int ret = 0;
    int rfd;
    int readsize;

    if (fstr_pp->search) {
        return 1;
    }

    if (fstr_pp->frmode == FRMODE_CD) {
        /* Search file from CD */
        ret = sceCdSearchFile(&fstr_pp->fpCd, fstr_pp->fname);
        if (ret) {
            fstr_pp->search = 1;
        } else {
            printf("CD SEARCH ERROR!![%s]\n", fstr_pp->fname);
        }
    } else {
        /* Search file from PC */
        rfd = sceOpen(fstr_pp->fname, SCE_RDONLY);
        if (rfd < 0) {
            printf("Can\'t open %s\n", fstr_pp->fname);
        } else {
            readsize = sceLseek(rfd, 0, SCE_SEEK_END);
            if (readsize < 0) {
                printf("Can\'t open %s\n", fstr_pp->fname);
            } else {
                fstr_pp->fpCd.size = readsize;
                fstr_pp->search = 1;

                ret = 1;
            }
        
            sceClose(rfd);
        }
    }

    return ret;
}

static int cdctrlReadSub(FILE_STR *fstr_pp, int ofs, int size, int buf) {
    if (fstr_pp->frmode == FRMODE_CD) {
        sceCdRMode cdmode   = { .spindlctrl = SCECdSpinNom };
        int        read_lsn = fstr_pp->fpCd.lsn + (ofs / 2048);

        if (!sceCdSeek(read_lsn)) {
            printf("sceCdSeek Error! %s\n", fstr_pp->fname);
            return 0;
        } 

        while (sceCdSync(1)) {
            MtcWait(1);
        }

        if (!sceCdRead(read_lsn, (size + 2047) / 2048, (void*)buf, &cdmode)) {
            printf("sceCdRead Error! %s\n", fstr_pp->fname);
            return 0;
        }

        while (sceCdSync(1)) {
            MtcWait(1);
        }

        if (sceCdGetError()) {
            return 0;
        }
            
        FlushCache(WRITEBACK_DCACHE);
        return 1;
    } else {
        int rfd = sceOpen(fstr_pp->fname, SCE_RDONLY);
        if (rfd < 0) {
            printf("Can\'t open %s\n", fstr_pp->fname);
            return 0;
        }

        if (sceLseek(rfd, ofs, SCE_SEEK_SET) < 0) {
            printf("Can\'t open %s\n", fstr_pp->fname);
            sceClose(rfd);
            return 0;
        }

        if (sceRead(rfd, (void*)buf, size) < 0) {
            printf("read error!! \n");
            sceClose(rfd);
            return 0;
        }

        sceClose(rfd);
        FlushCache(WRITEBACK_DCACHE);
        return 1;
    }

    return 0;
}

void intReadSub(void) {
    int     read_pos;
    u_char *read_tmp_pp;
    u_char *head_read_pp;

    read_pos = 0;
    head_read_pp = usrMalloc(8192);

    printf("intg in!!!\n");

    while (1) {
        /* Read a whole sector into our header buffer */
        while (!cdctrlReadSub(cdctrl_str.fstr_pp, read_pos, 2048, (int)head_read_pp)) {
            MtcWait(1);
        }

        /* Check if we loaded a valid INT */
        if (PACK(head_read_pp)->id != PACKINT_MAGIC) {
            printf("INT FILE ERROR!![%s]\n", cdctrl_str.fstr_pp->fname);
            while (1) {
                MtcWait(1);
            }
        }

        FlushCache(WRITEBACK_DCACHE);

        if (PACK(head_read_pp)->head_size > 2048) {
            while (!cdctrlReadSub(cdctrl_str.fstr_pp, read_pos + 2048, ((PACK(head_read_pp)->head_size - 1) / 2048) * 2048, (int)(head_read_pp + 2048))) {
                MtcWait(1);
            }
        }

        read_pos += PACK(head_read_pp)->head_size + PACK(head_read_pp)->name_size;
        FlushCache(WRITEBACK_DCACHE);

        /* Check if there's data present */
        if (PACK(head_read_pp)->data_size != 0) {
            /* If we don't have a temp buffer, allocate it ourselves */
            if (cdctrl_str.tmp_area != NULL) {
                read_tmp_pp = (u_char*)(cdctrl_str.tmp_area - PACK(head_read_pp)->data_size);
                printf("buf use:%08x size:%08x\n", cdctrl_str.tmp_area - PACK(head_read_pp)->data_size, PACK(head_read_pp)->data_size);
            } else {
                read_tmp_pp = usrMalloc(PACK(head_read_pp)->data_size);
            }

            /* Read the data... */
            FlushCache(WRITEBACK_DCACHE);
            while (!cdctrlReadSub(cdctrl_str.fstr_pp, read_pos, PACK(head_read_pp)->data_size, (int)read_tmp_pp)) {
                MtcWait(1);
            }

            /* ...and decode it */
            FlushCache(WRITEBACK_DCACHE);
            PackIntDecodeWait(read_tmp_pp, (u_char*)UsrMemAllocNext(), 230);
            FlushCache(WRITEBACK_DCACHE);

            if (cdctrl_str.tmp_area == NULL) {
                usrFree(read_tmp_pp);
            }

            read_pos += PACK(head_read_pp)->data_size;
        }

        /* Handle the decoded data */
        switch (PACK(head_read_pp)->ftype) {
        /* TIM2 textures */
        case FT_VRAM:
            PR_SCOPE
            int i;
            printf("int file tim2 file in\n");

            for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                Tim2Trans((void*)PACK(head_read_pp)->adr[i] + UsrMemAllocNext());
            }

            printf("int file tim2 file out\n");
            PR_SCOPEEND
            break;
        /* Sounds */
        case FT_SND:
            PR_SCOPE
            int i;
            printf("int file snd file in\n");

            /* TODO: name TAPCT commands */
            while (TapCt(0x8070, 0, 0)) {
                MtcWait(1);
            }

            for (i = 0; i < PACK(head_read_pp)->fnum / 2; i++) {
                TapCt(0x8030 | i, PACK(head_read_pp)->adr[i] + UsrMemAllocNext(),
                    PACK(head_read_pp)->adr[i + 1] - PACK(head_read_pp)->adr[i]);
                TapCt(0x8040 | i, PACK(head_read_pp)->adr[i + PACK(head_read_pp)->fnum / 2] + UsrMemAllocNext(),
                    PACK(head_read_pp)->adr[i + 1 + PACK(head_read_pp)->fnum / 2] - PACK(head_read_pp)->adr[i + PACK(head_read_pp)->fnum / 2]);
            }

            while (TapCt(0x8070, 0, 0)) {
                MtcWait(1);
            }

            printf("int file snd file out\n");
            PR_SCOPEEND
            break;
        /* PaRappa's hat textures (TIM2) */
        case FT_R1:
        case FT_R2:
        case FT_R3:
        case FT_R4:
            PR_SCOPE
            int i;

            if (GetHatRound() == (PACK(head_read_pp)->ftype - 4)) {
                printf("int file tim2 round:%d file in\n", PACK(head_read_pp)->ftype - 3);

                for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                    Tim2Trans((void*)PACK(head_read_pp)->adr[i] + UsrMemAllocNext());
                }

                printf("int file tim2 roud file out\n");
            }
            PR_SCOPEEND
            break;
        /* Models, animations, etc. */
        case FT_ONMEM:
            PR_SCOPE
            int i;
            printf("mem cnt [%d]\n", PACK(head_read_pp)->fnum);

            for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                UsrMemAlloc(PACK(head_read_pp)->adr[i + 1] - PACK(head_read_pp)->adr[i]);
            }
            PR_SCOPEEND
        /* Fallthrough, stops after the ONMEM file category */
        default:
            printf("intg exit!!\n");
            usrFree(head_read_pp);
            return;
        }
    }
}

void cdctrlReadData(void *x) {
    while (!CdctrlSerch(cdctrl_str.fstr_pp)) {
        MtcWait(1);
    }

    switch (cdctrl_str.fstr_pp->ftmode) {
    case FTMODE_INTG:
        intReadSub();
        current_intg_adrs = (void*)cdctrl_str.read_area;
        break;
    case FTMODE_ETC:
        while (!cdctrlReadSub(cdctrl_str.fstr_pp, 0, cdctrl_str.fstr_pp->fpCd.size, cdctrl_str.read_area)) {
            MtcWait(1);
        }
        break;
    }

    FlushCache(WRITEBACK_DCACHE);
    MtcWait(1);

    cdctrl_str.status = 0;
    MtcExit();
}

static void cdctrlReadDataOne(void *x) {
    while (!CdctrlSerch(cdctrl_str.fstr_pp)) {
        MtcWait(1);
    }

    while (!cdctrlReadSub(cdctrl_str.fstr_pp, 0, cdctrl_str.fstr_pp->fpCd.size, cdctrl_str.read_area)) {
        MtcWait(1);
    }

    MtcWait(1);
    cdctrl_str.status = 0;
    MtcExit();
}

void CdctrlRead(FILE_STR *fstr_pp, u_int buf, int tmpbuf) {
    cdctrl_str.status = 1;
    cdctrl_str.error_status = 0;
    cdctrl_str.read_area = buf;
    cdctrl_str.tmp_area = tmpbuf;
    cdctrl_str.fstr_pp = fstr_pp;
    MtcExec(cdctrlReadData, MTC_TASK_CDCTRL);
}

void CdctrlReadOne(FILE_STR *fstr_pp, u_int buf, int tmpbuf) {
    cdctrl_str.status = 1;
    cdctrl_str.error_status = 0;
    cdctrl_str.read_area = buf;
    cdctrl_str.tmp_area = tmpbuf;
    cdctrl_str.fstr_pp = fstr_pp;
    MtcExec(cdctrlReadDataOne, MTC_TASK_CDCTRL);
}

// さき(saki) -> Destination / もと(moto) -> Source
void usrMemcpy(void *sakip, void *motop, int size) {
    int     i = size / sizeof(int);
    int *m_pp = motop;
    int *s_pp = sakip;

    while (--i != -1) {
        *s_pp++ = *m_pp++;
    }
}

void CdctrlMemIntgDecode(u_int rbuf, u_int setbuf) {
    u_char *head_read_pp;
    u_int   next_rp;

    current_intg_adrs = (void*)setbuf;
    head_read_pp      = usrMalloc(8192);
    next_rp           = PACK(rbuf)->head_size;

    while (1) {
        /* Copy the INT to our buffer */
        usrMemcpy(head_read_pp, (void*)rbuf, PACK(rbuf)->head_size);
        FlushCache(WRITEBACK_DCACHE);
        
        /* Check if we loaded a valid INT */
        if (PACK(head_read_pp)->id != PACKINT_MAGIC) {
            printf("INT FILE ERROR!![%s]\n", cdctrl_str.fstr_pp->fname);
            while (1) {
                MtcWait(1);
            }
        }

        rbuf += PACK(head_read_pp)->head_size + PACK(head_read_pp)->name_size;
        FlushCache(WRITEBACK_DCACHE);

        /* Check if there's data present */
        if (PACK(head_read_pp)->data_size != 0) {
            /* Decode the data */
            PackIntDecodeWait((u_char*)rbuf, (u_char*)UsrMemAllocNext(), 230);
            rbuf += PACK(head_read_pp)->data_size;
        }

        FlushCache(WRITEBACK_DCACHE);

        /* Handle the decoded data */
        switch (PACK(head_read_pp)->ftype) {
        /* TIM2 textures */
        case FT_VRAM:
            PR_SCOPE
            int i;
            printf("tim trans in\n");

            for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                Tim2Trans((void*)PACK(head_read_pp)->adr[i] + UsrMemAllocNext());
            }

            printf("tim trans out\n");
            PR_SCOPEEND
            break;
        /* Sounds */
        case FT_SND:
            PR_SCOPE
            int i;

            for (i = 0; i < PACK(head_read_pp)->fnum / 2; i++) {
                /* TODO: name TAPCT commands */
                TapCt(0x8030 | i, PACK(head_read_pp)->adr[i] + UsrMemAllocNext(),
                    PACK(head_read_pp)->adr[i+1] - PACK(head_read_pp)->adr[i]);
                TapCt(0x8040 | i, PACK(head_read_pp)->adr[i + PACK(head_read_pp)->fnum / 2] + UsrMemAllocNext(),
                    PACK(head_read_pp)->adr[i+1+PACK(head_read_pp)->fnum / 2] - PACK(head_read_pp)->adr[i + PACK(head_read_pp)->fnum / 2]);
            }

            while (TapCt(0x8070, 0, 0)) {
                MtcWait(1);
            }
            PR_SCOPEEND
            break;
        /* PaRappa's hat textures (TIM2) */
        case FT_R1:
        case FT_R2:
        case FT_R3:
        case FT_R4:
            PR_SCOPE
            int i;

            if (GetHatRound() == PACK(head_read_pp)->ftype - 4) {
                printf("int file tim2 round:%d file in\n", PACK(head_read_pp)->ftype - 3);

                for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                    Tim2Trans((void*)PACK(head_read_pp)->adr[i] + UsrMemAllocNext());
                }

                printf("int file tim2 roud file out\n");
            }

            PR_SCOPEEND
            break;
        /* Models, animations, etc. */
        case FT_ONMEM:
            PR_SCOPE
            int i;
            printf("mem cnt [%d]\n", PACK(head_read_pp)->fnum);

            for (i = 0; i < PACK(head_read_pp)->fnum; i++) {
                UsrMemAlloc(PACK(head_read_pp)->adr[i+1] - PACK(head_read_pp)->adr[i]);
            }

            PR_SCOPEEND
            break;
        default:
            printf("intg exit!!\n");
            usrFree(head_read_pp);
            return;
        }
    }
}

int CdctrlStatus(void) {
    return cdctrl_str.status;
}

void CdctrlReadWait(void) {
    do {
        MtcWait(1);
    } while (CdctrlStatus());  
}

u_int CdctrlGetFileSize(FILE_STR *fstr_pp) {
    while (!CdctrlSerch(fstr_pp)) {
        MtcWait(1);
    }

    return fstr_pp->fpCd.size;
}

void CdctrlWP2SetChannel(u_char Lchan, u_char Rchan) {
    if (cdctrl_str.wp2chan[0] == Lchan &&
        cdctrl_str.wp2chan[1] == Rchan) {
        return;
    }

    WP2Ctrl(WP2_SETCHANNEL, PR_CONCAT(Lchan, Rchan));
    cdctrl_str.wp2chan[0] = Lchan;
    cdctrl_str.wp2chan[1] = Rchan;

    printf("channel change L[%d] R[%d]\n", Lchan, Rchan);
}

void CdctrlWP2Set(FILE_STR *fstr_pp) {
    cdctrl_str.status = 1;
    cdctrl_str.error_status = 0;
    cdctrl_str.fstr_pp = fstr_pp;
    
    WP2Ctrl(WP2_SETMODE, fstr_pp->mchan);

    while (!CdctrlSerch(cdctrl_str.fstr_pp));

    WP2Ctrl(WP2_SETTRPOINT, WP2_NONE);
    if (fstr_pp->frmode == FRMODE_PC) {
        WP2Ctrl(WP2_OPENFLOC, (int)cdctrl_str.fstr_pp->fname);
    } else {
        WP2Ctrl(WP2_OPENFLOC, (int)&cdctrl_str.fstr_pp->fpCd);
    }

    CdctrlWP2SetVolume(0);
    
    cdctrl_str.read_area = WP2Ctrl(WP2_GETTIMESAMPLE, 0);
    cdctrl_str.wp2chan[0] = 0;
    cdctrl_str.wp2chan[1] = 1;

    WP2Ctrl(WP2_SETCHANNEL, PR_CONCAT(cdctrl_str.wp2chan[0], cdctrl_str.wp2chan[1]));
    WP2Ctrl(WP2_PRELOADBACK, WP2_NONE);
}

void CdctrlWP2SetFileSeek(FILE_STR *fstr_pp, int seek_pos) {
    /* Stop the current WP2 playing */
    CdctrlWP2SetVolume(0);
    WP2Ctrl(WP2_STOP, WP2_NONE);

    if (cdctrl_str.fstr_pp != fstr_pp) {
        cdctrl_str.fstr_pp = fstr_pp;
        printf("!!max chan [%d]\n", fstr_pp->mchan);

        WP2Ctrl(WP2_SETMODE, cdctrl_str.fstr_pp->mchan);
        if (fstr_pp->frmode == FRMODE_PC) {
            WP2Ctrl(WP2_SEEKFLOC, (int)cdctrl_str.fstr_pp->fname);
        } else {
            WP2Ctrl(WP2_SEEKFLOC, (int)&cdctrl_str.fstr_pp->fpCd);
        }

        cdctrl_str.wp2chan[1] = 0;
        cdctrl_str.wp2chan[0] = 0;
    }

    /* Seek and preload */
    WP2Ctrl(WP2_SEEK, seek_pos);
    WP2Ctrl(WP2_PRELOADBACK, WP2_NONE);
}

void CdctrlWP2SetFileSeekChan(FILE_STR *fstr_pp, int seek_pos, u_char Lch, u_char Rch) {
    /* Stop the current WP2 playing */
    CdctrlWP2SetVolume(0);
    WP2Ctrl(WP2_STOP, WP2_NONE);

    cdctrl_str.fstr_pp = fstr_pp;
    printf("!!max chan [%d]\n", fstr_pp->mchan);

    WP2Ctrl(WP2_SETMODE, cdctrl_str.fstr_pp->mchan);
    if (fstr_pp->frmode == FRMODE_PC) {
        WP2Ctrl(WP2_SEEKFLOC, (int)cdctrl_str.fstr_pp->fname);
    } else {
        WP2Ctrl(WP2_SEEKFLOC, (int)&cdctrl_str.fstr_pp->fpCd);
    }

    cdctrl_str.wp2chan[1] = 0;
    cdctrl_str.wp2chan[0] = 0;

    /* Seek, set the channels, and preload */
    WP2Ctrl(WP2_SEEK, seek_pos);
    CdctrlWP2SetChannel(Lch, Rch);
    WP2Ctrl(WP2_PRELOADBACK, WP2_NONE);
}

void CdctrlWP2Seek(int seek_pos) {
    WP2Ctrl(WP2_STOP, WP2_NONE);
    WP2Ctrl(WP2_SEEK, seek_pos);
    WP2Ctrl(WP2_PRELOAD, WP2_NONE);
}

void CdctrlWP2Play(void) {
    WP2Ctrl(WP2_START, WP2_NONE);
}

int CdctrlWP2PlayEndCheck(void) {
    return WP2Ctrl(WP2_GETMODE, WP2_NONE) & WP2_MODE_TERMINATE;
}

int CdctrlWP2CheckBuffer(void) {
    return WP2Ctrl(WP2_READBUFF, WP2_NONE);
}

void CdctrlWP2SetVolume(u_short vol) {
    int volume;

    cdctrl_str.volume = vol;
    vol *= 256;
    
    if ((short)vol < 0) {
        volume = 0x7fff;
    } else {
        volume = vol;
    }

    WP2Ctrl(WP2_SETVOLDIRECT, PR_CONCAT(volume, volume));
}

u_short CdctrlWP2GetVolume(void) {
    return cdctrl_str.volume;
}

void CdctrlWp2Stop(void) {
    WP2Ctrl(WP2_STOP, WP2_NONE);
}

void CdctrlWp2FileEnd(void) {
    WP2Ctrl(WP2_STOP, WP2_NONE);
    WP2Ctrl(WP2_CLOSE, WP2_NONE);
}

int CdctrlWp2GetSample(void) {
    return WP2Ctrl(WP2_GETTIME, WP2_NONE);
}

int CdctrlWp2GetFrame(void) {
    return (WP2Ctrl(WP2_GETTIME, WP2_NONE) * 24) / 75;
}

int CdctrlFrame2WP2sample(int frame) {
    return (frame * 75) / 24;
}

long CdctrlWp2GetSndTime(float tempo) {
    float beat = WP2Ctrl(WP2_GETTIME, WP2_NONE);

    beat = (tempo * 16.0f * beat) / 1875.0f;
    return (int)beat;
}

long CdctrlSndTime2WP2sample(float tempo, long beat) {
    return (beat * 1875.0f) / (tempo * 16.0f);
}

void CdctrlWp2GetSampleTmpBuf(void) {
    static int time_tmp_max = 0;
    int time_tmp = *T0_COUNT;

    cdSampleTmp = WP2Ctrl(WP2_GETTIME, WP2_NONE);
    time_tmp = *T0_COUNT - time_tmp;

    if (time_tmp > time_tmp_max) {
        printf("max cd time get[%d]\n", time_tmp);
        time_tmp_max = time_tmp;
    }
}

int CdctrlWp2GetSampleTmp(void) {
    return cdSampleTmp;
}

int CdctrlWp2GetFrameTmp(void) {
    int frame = cdSampleTmp;
    return (frame * 24) / 75;
}

long CdctrlWp2GetSndTimeTmp(float tempo) {
    float beat = cdSampleTmp;

    beat = (beat * tempo * 16.0f) / 1875.0f;
    return (int)beat;
}

long CdctrlWp2CdSample2SndTime(long samplecnt, float tempo) {
    float beat = samplecnt;

    beat = (beat * tempo * 16.0f) / 1875.0f;
    return (int)beat;
}

long CdctrlWp2CdSample2Frame(long samplecnt) {
    return (samplecnt * 24) / 75;
}

void CdctrlXTRset(FILE_STR *fstr_pp, u_int usebuf) {
    int       i;
    TRBOX_TR *tb_pp;
    u_char   *pr_pp;
    int       seek_pos;

    cdctrl_str.status = 1;
    cdctrl_str.error_status = 0;
    cdctrl_str.fstr_pp = fstr_pp;

    /* Search the XTR file */
    while (!CdctrlSerch(cdctrl_str.fstr_pp));

    /* Read the XTR's header (1 sector) */
    while (!cdctrlReadSub(fstr_pp, 0, 2048, usebuf)) {
        MtcWait(1);
    }
    
    FlushCache(WRITEBACK_DCACHE);

    if (STR(usebuf)->read_size > 2048) {
        while (!cdctrlReadSub(fstr_pp, 2048, STR(usebuf)->read_size - 2048, (int)(usebuf + 2048))) {
            MtcWait(1);
        }
    }

    FlushCache(WRITEBACK_DCACHE);

    if (STR(usebuf)->channel != 0) {
        cdctrl_str.fstr_pp->mchan = STR(usebuf)->channel;
    } else {
        cdctrl_str.fstr_pp->mchan = 6;
    }

    WP2Ctrl(WP2_SETMODE, cdctrl_str.fstr_pp->mchan);
    FlushCache(WRITEBACK_DCACHE);

    tb_pp = STR(usebuf)->trbox_tr;
    for (i = 0; i < PR_ARRAYSIZE(STR(usebuf)->trbox_tr); i++, tb_pp++) {
        if (tb_pp->press_size == 0) {
            continue;
        }

        pr_pp = (u_char*)UsrMemEndAlloc(tb_pp->press_size);
        UsrMemEndFree();
        FlushCache(WRITEBACK_DCACHE);

        while (!cdctrlReadSub(fstr_pp, tb_pp->read_pos, tb_pp->press_size, (int)pr_pp)) {
            MtcWait(1);
        }

        FlushCache(WRITEBACK_DCACHE);

        printf("dec size[%08x]\n", PackIntGetDecodeSize(pr_pp));
        printf("dec info trpos[%08x] read_pos[%08x] press_size[%08x]\n", tb_pp->trpos, tb_pp->read_pos, tb_pp->press_size);

        PackIntDecodeWait(pr_pp, (u_char*)(tb_pp->trpos + usebuf), 230);
    }

    FlushCache(WRITEBACK_DCACHE);
    
    seek_pos = STR(usebuf)->seek;

    printf("P3STR INIT REQ IN\n");
    p3StrInit(usebuf);
    printf("P3STR INIT REQ OUT\n");

    WP2Ctrl(WP2_SETTRPOINT, usebuf);
    printf("WP2_BGMSETTRPOINT exit\n");

    if (fstr_pp->frmode == FRMODE_PC) {
        WP2Ctrl(WP2_OPENFLOC, (int)cdctrl_str.fstr_pp->fname);
    } else {
        WP2Ctrl(WP2_OPENFLOC, (int)&cdctrl_str.fstr_pp->fpCd);
    }

    WP2Ctrl(WP2_SEEK, seek_pos);
    printf("WP2_SEEK exit\n");

    WP2Ctrl(WP2_SETCHANNEL, 1);
    WP2Ctrl(WP2_PRELOADBACK, WP2_NONE);
    printf("WP2_PRELOAD exit\n");

    CdctrlWP2SetVolume(0);

    cdctrl_str.read_area  = WP2Ctrl(WP2_GETTIMESAMPLE, WP2_NONE);
    cdctrl_str.wp2chan[0] = 0;
    cdctrl_str.wp2chan[1] = 1;
}

void* GetIntAdrsCurrent(u_short num) {
    return (void*)UsrMemGetAdr(num);
}

int GetIntSizeCurrent(u_short num) {
    return UsrMemGetSize(num);
}

static int sndFadeTime = 0;

static void cdctrlSndFadeOut(void *x) {
    int timer;

    for (timer = 0; timer < sndFadeTime; timer++) {
        CdctrlMasterVolSet(((sndFadeTime - timer) * 0x3fff) / sndFadeTime);
        MtcWait(1);
    }

    sndFadeTime = 0;
    MtcExit();
}

void CdctrlSndFadeOut(int time) {
    sndFadeTime = time;
    MtcExec(cdctrlSndFadeOut, MTC_TASK_CDCTRL);
}

int CdctrlSndFadeOutCheck(void) {
    return (sndFadeTime == 0);
}

void CdctrlSndFadeOutWait(int time) {
    CdctrlSndFadeOut(time);
    
    while (!CdctrlSndFadeOutCheck()) {
        MtcWait(1);
    }
}

/* VRAM 0x00399374 on the NTSC-J proto */
int _sdata_pad_cdctrl_ = 0;
