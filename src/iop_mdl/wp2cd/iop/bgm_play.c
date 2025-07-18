#include "../wp2cd.h"

#include <intrman.h>
#include <libcdvd.h>
#include <libsd.h>
#include <sif.h>
#include <sysmem.h>
#include <thread.h>

#include <sys/fcntl.h>
#include <sys/types.h>
#include <stdio.h>

#define KB(x) (x*1024)
#define SCTORS(x) (x/2048)

#define RDMODE_CD (0)
#define RDMODE_PC (1)

int  _PreLoadBack(int status);
void _BgmStop(void);
int  _BgmPlay(int status);

typedef struct { // 0x30
    /* 0x00:0 */ unsigned int size : 32;
    /* 0x04:0 */ unsigned int pos : 32;
    /* 0x08:0 */ unsigned int ofs : 32;
    /* 0x0c:0 */ unsigned int Channel : 32;
    /* 0x10 */ u_short ReqChan[2];
    /* 0x14 */ int TransPos;
    /* 0x18 */ int TransMax;
    /* 0x1c */ int Tr1Size;
    /* 0x20 */ int StartTrPos;
    /* 0x24 */ int TransEEAdrs;
    /* 0x28 */ int TransId;
    /* 0x2c */ int readBackFlag;
} WAVEP2;

typedef struct { // 0x10
    /* 0x0 */ int buf_pos[2];
    /* 0x8 */ int TrackSize;
    /* 0xc */ int dbuf_flg;
} SBUF;

typedef struct { // 0x10
    /* 0x0 */ u_int trSize;
    /* 0x4 */ u_int trAdr;
    /* 0x8 */ u_int pad1;
    /* 0xc */ u_int pad2;
    /* 0x10 */ u_char dat[0];
} P3STR_TRH;

/* data 20 */ extern /* static */ int bug_bug_bug_flag;
/* data 24 */ extern /* static */ int fp_pc;
/* data 28 */ extern SBUF sbuf;
/* data 38 */ extern int gThid;
/* data 3c */ extern int gSem;
/* data 40 */ extern int gThid_Tr;
/* data 44 */ extern int gSem_Tr;
/* data 48 */ extern int gBgmIntr;
/* data 4c */ extern int gBgmIntrTime;
/* data 50 */ extern int ReadBuff;
/* data 54 */ extern u_int ReadOutCnt;
/* data 58 */ extern u_int ReadOddEven;
/* data 5c */ extern int BgmVolumeSet;
/* data 60 */ extern int BgmPause;
/* data 64 */ extern int BgmMode;
/* bss 14 */  extern int oldstat;
/* bss 1b0 */ extern unsigned char ring_buf[524288];
/* bss 0 */   extern /* static */ int bgmPlayReadMode;
/* bss 150 */ extern WAVEP2 wavep2;
/* bss 10 */  extern int ReadBuffSize;
/* bss 0 */   extern int ReadPkSize;
/* bss 8 */   extern int BgmVolume;
/* bss 180 */ extern sceCdlFILE fpCd;
/* bss 4 */   extern sceCdRMode modeCd;
/* bss c */   extern int CdErrCode;
/* bss 8 */   extern /* static */ sceSifDmaData sifdmadata[16];

int Wp2CdStRead(u_int sectors, u_int *buf, u_int mode, u_int *err) {
    int ret;
    int read_sectors;
    int now_read;

    ret = 0;
    read_sectors = sectors;

    while (1) {
        if (!(BgmMode & 0x800)) {
            return ret;
        }

        now_read = sceCdStRead(read_sectors, buf, 0, err);
        ret = ret + now_read;

        if (ret == sectors) {
            break;
        }

        buf += (now_read * 512);
        read_sectors = sectors - ret;
        
        sceCdGetError();
        if (*err != 0) {
            /* Empty */
        }
    }

    return ret;
}

int readPC(int fp, u_char *buf, int size) {
    int ret;

    while (1) {
        ret = read(fp, buf, size);
        if (ret == size) {
            break;
        }
        if (ret < 0) {
            continue;
        }

        size -= ret;
        buf  += ret;
    }


    return 0;
}

void usrMemcpy(void *sakip, void *motop, int size) {
    int     i = size / sizeof(int);
    int *m_pp = motop;
    int *s_pp = sakip;

    while (--i != -1) {
        *s_pp++ = *m_pp++;
    }
}

int TransBufSet(void) {
    int i;
    int pos;
    int spu_pp;
    int org;
    int ee_cnt;

    ee_cnt = 0;
    pos = wavep2.TransPos;
    org = pos;
    spu_pp = sbuf.buf_pos[sbuf.dbuf_flg];

    for (i = 0; i < (sbuf.TrackSize / 2); i++) {
        usrMemcpy((void*)spu_pp, (void*)(((pos * wavep2.Tr1Size) + (wavep2.ReqChan[0] * 512)) + ReadBuff), 512);
        spu_pp += 512;
        usrMemcpy((void*)spu_pp, (void*)(((pos * wavep2.Tr1Size) + (wavep2.ReqChan[1] * 512)) + ReadBuff), 512);
        spu_pp += 512;

        if (wavep2.TransEEAdrs != 0) {
            int j;
            for (j = 0; j < wavep2.Channel; j++) {
                P3STR_TRH *tr_pp;

                if (wavep2.ReqChan[0] == j || wavep2.ReqChan[1] == j) {
                    continue;
                }

                tr_pp = (P3STR_TRH*)(((pos * wavep2.Tr1Size) + (j * 512)) + ReadBuff);
                if (tr_pp->trSize == 0) {
                    continue;
                }

                sifdmadata[ee_cnt].data = (unsigned int)&tr_pp->dat;
                sifdmadata[ee_cnt].addr = tr_pp->trAdr + wavep2.TransEEAdrs;
                sifdmadata[ee_cnt].size = tr_pp->trSize;
                sifdmadata[ee_cnt].mode = 0;

                ee_cnt++;
                ee_cnt %= 16;
                if (ee_cnt == 0) {
                    int ret = 0;
                    while (ret == 0) {
                        CpuSuspendIntr(&oldstat);
                        ret = sceSifSetDma(sifdmadata, 16);
                        if (ret != 0) {
                            wavep2.TransId = ret;
                        }
                        CpuResumeIntr(oldstat);
                    }
                }
            }
        }

        pos++;
        if (pos >= wavep2.TransMax) {
            pos = 0;
        }
    }

    if (ee_cnt != 0) {
        int ret = 0;
        while (ret == 0) {
            CpuSuspendIntr(&oldstat);
            ret = sceSifSetDma(sifdmadata, ee_cnt);
            if (ret != 0) {
                wavep2.TransId = ret;
            }
            CpuResumeIntr(oldstat);
        }
    }

    sbuf.dbuf_flg ^= 1;
    wavep2.TransPos = pos;

    return (pos / (wavep2.TransMax / 2)) != (org / (wavep2.TransMax / 2));
}

static int IntFunc(int ch, void *common) {
    if (gBgmIntr == 0) {
        iSignalSema(gSem);
        gBgmIntr = 1;
        ReadOddEven++;
    } else {
        sbuf.dbuf_flg ^= 1;
        bug_bug_bug_flag++;
    }

    gBgmIntrTime = 1;
    return 1;
}

static int IntFuncEnd(int ch, void *common) {
    return 0;
}

static int makeMyThread() {
    struct ThreadParam param;
    int thid;

    param.attr         = TH_C;
    param.entry        = _BgmPlay;
    param.initPriority = 0x58;
    param.stackSize    = 0x800;
    param.option       = 0;

    thid = CreateThread(&param);
    return thid;
}

static int makeMyThread_Tr() {
    struct ThreadParam param;
    int thid;

    param.attr         = TH_C;
    param.entry        = _PreLoadBack;
    param.initPriority = 0x5a;
    param.stackSize    = 0x800;
    param.option       = 0;

    thid = CreateThread(&param);
    return thid;
}

static int makeMySem() {
    struct SemaParam sem;

    sem.initCount = 0;
    sem.maxCount  = 1;
    sem.attr      = AT_THFIFO;
    return CreateSema(&sem);
}

void BgmSetVolumeDirect(unsigned int vol) {
    CpuSuspendIntr(&oldstat);
    sceSdSetParam(0xf81,  vol >> 16);
    sceSdSetParam(0x1081, vol);
    CpuResumeIntr(oldstat);
    return; /* Explicit return needed to match. */
}

void BgmSetMasterVolume(unsigned int vol) {
    CpuSuspendIntr(&oldstat);
    sceSdSetParam(0x981, vol >> 16);
    sceSdSetParam(0xa81, vol);
    CpuResumeIntr(oldstat);
    return; /* Explicit return needed to match. */
}

void BgmCdInit(int mode) {
    sceCdInit(SCECdINIT);
    sceCdMmode(mode);
    sceCdStInit(SCTORS(KB(512)), 16, (u_int)ring_buf);
    sceCdDiskReady(0); /* blocking */
}

void BgmSdInit(int status) {
    sceSdInit(0);
    sceSdSetCoreAttr(SD_C_SPDIF_MODE, SD_SPDIF_MEDIA_DVD | SD_SPDIF_COPY_PROHIBIT | SD_SPDIF_OUT_OFF);
    return; /* Explicit return needed to match. */
}

int BgmInit(int block_size) {
    int     i;
    u_char *clr_pp = (u_char*)&wavep2;

    for (i = 0; i < sizeof(wavep2); i++) {
        /* BUG: Always clearing the same byte... */
        *clr_pp = 0;
    }

    if (gSem == 0) {
        gSem = makeMySem();
    }

    if (gThid == 0) {
        gThid = makeMyThread();
        StartThread(gThid, NULL);
    }

    if (gSem_Tr == 0) {
        gSem_Tr = makeMySem();
    }

    if (gThid_Tr == 0) {
        gThid_Tr = makeMyThread_Tr();
        StartThread(gThid_Tr, NULL);
    }

    if (sbuf.buf_pos[0] == NULL) {
        CpuSuspendIntr(&oldstat);
        sbuf.buf_pos[0] = (int)AllocSysMemory(SMEM_Low, KB(44), NULL);
        CpuResumeIntr(oldstat);

        /* buf_pos[1] -> Next 22KB */
        sbuf.buf_pos[1] = sbuf.buf_pos[0] + 0x5800;
        sbuf.TrackSize = 44;
    }

    sbuf.dbuf_flg = 0;

    if (ReadBuff != NULL) {
        CpuSuspendIntr(&oldstat);
        FreeSysMemory((void*)ReadBuff);
        CpuResumeIntr(oldstat);
    }

    CpuSuspendIntr(&oldstat);
    ReadBuff = (int)AllocSysMemory(SMEM_Low, block_size * 512, NULL);
    CpuResumeIntr(oldstat);

    ReadBuffSize = block_size * 512;
    ReadOutCnt = 0;

    return gThid;
}

void BgmQuit(int status) {
    CpuSuspendIntr(&oldstat);

    FreeSysMemory((void*)ReadBuff);
    ReadBuff = 0;
    ReadBuffSize = 0;

    FreeSysMemory((void*)sbuf.buf_pos[0]);
    sbuf.buf_pos[0] = NULL;
    sbuf.buf_pos[1] = NULL;
    sbuf.TrackSize = 0;

    CpuResumeIntr(oldstat);

    if (gThid != 0) {
        TerminateThread(gThid);
    }

    if (gSem != 0) {
        DeleteSema(gSem);
    }

    if (gThid != 0) {
        DeleteThread(gThid);
    }

    gThid = 0;
    gSem = 0;

    if (gThid_Tr != 0) {
        TerminateThread(gThid_Tr);
    }

    if (gSem_Tr != 0) {
        DeleteSema(gSem_Tr);
    }

    if (gThid_Tr != 0) {
        DeleteThread(gThid_Tr);
    }

    gThid_Tr = 0;
    gSem_Tr = 0;
    return; /* Explicit return needed to match. */
}

int BgmOpen(char *filename) {
    int ret;

    if (filename[0] == 'h' && filename[1] == 'o' &&
        filename[2] == 's' && filename[3] == 't') {
        bgmPlayReadMode = 1;
    } else {
        bgmPlayReadMode = 0;
    }

    if (bgmPlayReadMode != RDMODE_CD) {
        if (fp_pc >= 0) {
            close(fp_pc);
        }

        fp_pc = open(filename, O_RDONLY);

        if (fp_pc < 0) {
            ret = FALSE;
        } else {
            ret = TRUE;
            fpCd.size = lseek(fp_pc, 0, SEEK_END);
            fpCd.lsn = 0;
            lseek(fp_pc, 0, SEEK_SET);
        }
    } else {
        ret = sceCdSearchFile(&fpCd, filename);
    }

    if (!ret) {
        return -1;
    }

    sceCdDiskReady(0); /* blocking */

    modeCd.trycount = 0; /* 256 tries */
    modeCd.spindlctrl = SCECdSpinStm;
    modeCd.datapattern = SCECdSecS2048;

    ReadOutCnt = 0;
    gBgmIntr = 0;

    wavep2.size = fpCd.size / 2048;
    wavep2.ofs = 0;
    wavep2.pos = 0;
    wavep2.StartTrPos = 0;
    wavep2.TransPos = 0;

    if (bgmPlayReadMode == RDMODE_CD) {
        while (!sceCdStStart(fpCd.lsn, &modeCd));
    }

    BgmMode |= 0x800;
    ReadOddEven = 0;

    CpuSuspendIntr(&oldstat);
    sceSdSetTransIntrHandler(1, IntFunc, NULL);
    CpuResumeIntr(oldstat);

    wavep2.TransId = 0;
    wavep2.readBackFlag = FALSE;

    return wavep2.size;
}

int BgmOpenFLoc(sceCdlFILE *fpLoc) {
    u_char *filename;
    int     ret;

    filename = (u_char*)fpLoc;

    if (filename[0] == 'h' && filename[1] == 'o' &&
        filename[2] == 's' && filename[3] == 't') {
        bgmPlayReadMode = RDMODE_PC;
    } else {
        bgmPlayReadMode = RDMODE_CD;
    }

    gBgmIntr = 0;

    if (bgmPlayReadMode != RDMODE_CD) {
        if (fp_pc >= 0) {
            close(fp_pc);
        }

        do {
            fp_pc = open(filename, O_RDONLY);
        } while (fp_pc < 0);

        ret = 1;

        fpCd.size = lseek(fp_pc, 0, SEEK_END);
        fpCd.lsn = 0;
        lseek(fp_pc, 0, SEEK_SET);
    } else {
        fpCd = *fpLoc;
    }

    sceCdDiskReady(0); /* blocking */

    modeCd.trycount = 0; /* 256 tries */
    modeCd.spindlctrl = SCECdSpinStm;
    modeCd.datapattern = SCECdSecS2048;

    ReadOutCnt = 0;

    wavep2.size = fpCd.size / 2048;
    wavep2.ofs = 0;
    wavep2.pos = 0;
    wavep2.StartTrPos = 0;
    wavep2.TransPos = 0;

    if (bgmPlayReadMode == RDMODE_CD) {
        while (!sceCdStStart(fpCd.lsn, &modeCd));
    }

    BgmMode |= 0x800;
    ReadOddEven = 0;

    CpuSuspendIntr(&oldstat);
    sceSdSetTransIntrHandler(1, IntFunc, NULL);
    CpuResumeIntr(oldstat);

    wavep2.TransId = 0;
    wavep2.readBackFlag = FALSE;

    return wavep2.size;
}

void BgmClose(int status) {
    ReadOddEven = 0;

    CpuSuspendIntr(&oldstat);
    sceSdSetTransIntrHandler(1, IntFuncEnd, NULL);
    CpuResumeIntr(oldstat);

    BgmMode &= ~0x800;

    if (bgmPlayReadMode == RDMODE_CD) {
        while (!sceCdStStop());
    } else {
        if (fp_pc >= 0) {
            close(fp_pc);
        }

        fp_pc = -1;
    }

    if (wavep2.TransEEAdrs != NULL && wavep2.TransId != 0) {
        while (1) {
            CpuSuspendIntr(&oldstat);
            if (sceSifDmaStat(wavep2.TransId) < 0) {
                break;
            }
            CpuResumeIntr(oldstat);
        }

        CpuResumeIntr(oldstat);
    }

    wavep2.TransId = 0;
    wavep2.readBackFlag = FALSE;

    gBgmIntr = 0;

    TerminateThread(gThid_Tr);
    StartThread(gThid_Tr, NULL);
    return; /* Explicit return needed to match. */
}

static void TransBufSet_SUB(void) {
    int which;
    int remain;
    int tmp_size;
    int *addr;
    int i;

    if (!TransBufSet()) {
        return;
    }

    which    = 1 - (wavep2.TransPos / (wavep2.TransMax / 2));
    tmp_size = (wavep2.TransMax * wavep2.Tr1Size) / 2;
    remain   = wavep2.size - wavep2.pos;
    remain  *= 2048;

    if (remain > tmp_size) {
        remain = tmp_size;
    } else {
        addr = (int*)((((which * tmp_size) * 4) + ReadBuff) + (remain * 4));
        for (i = 0; i < ((u_int)(tmp_size - remain) / 4); i++) {
            *addr++ = 0;
        }

        BgmMode &= 0xfff;
        BgmMode |= 0x8000;
    }

    if (bgmPlayReadMode == RDMODE_CD) {
        if (Wp2CdStRead(remain / 2048, (u_int*)((which * tmp_size) + ReadBuff), 1, &CdErrCode) != (remain / 2048)) {
            /* Empty */
        }
    } else {
        readPC(fp_pc, (u_char*)((which * tmp_size) + ReadBuff), remain);
        CdErrCode = 0;
    }

    if (CdErrCode != 0) {
        /* Empty */
    }

    wavep2.pos += (remain / 2048);
}

int BgmPreLoad(void) {
    if (bgmPlayReadMode == RDMODE_CD) {
        Wp2CdStRead((wavep2.TransMax * wavep2.Tr1Size) / 2048, (u_int*)ReadBuff, 1, &CdErrCode);
    } else {
        readPC(fp_pc, (u_char*)ReadBuff, wavep2.TransMax * wavep2.Tr1Size);
        CdErrCode = 0;
    }

    if (!(BgmMode & 0x800)) {
        return 0;
    }

    wavep2.TransPos = 0;

    ReadOutCnt = ((wavep2.pos * 2048) / wavep2.Tr1Size) + wavep2.TransPos;

    wavep2.pos += (wavep2.TransMax * wavep2.Tr1Size) / 2048;

    sbuf.dbuf_flg = 0;

    TransBufSet_SUB();
    TransBufSet_SUB();

    return CdErrCode;
}

int _PreLoadBack(int status) {
    while (1) {
        WaitSema(gSem_Tr);

        if (BgmMode & 0x800) {
            BgmPreLoad();
        } else {
            /* Empty */
        }

        wavep2.readBackFlag = FALSE;
    }
}

void BgmPreLoadBack(void) {
    SignalSema(gSem_Tr);
    wavep2.readBackFlag = TRUE;
}

int BgmReadBuffFull(void) {
    if (wavep2.readBackFlag) {
        return 1;
    }

    if (bgmPlayReadMode != RDMODE_CD) {
        return 0;
    }

    if (sceCdStStat() < 240) {
        return 2;
    }

    return 0;
}

int BgmStart(void) {
    int ret;

    BgmSetVolumeDirect(BgmVolume);

    while (1) {
        CpuSuspendIntr(&oldstat);
        ret = sceSdBlockTrans(1, 0x10, (u_char*)sbuf.buf_pos[0], KB(44));
        CpuResumeIntr(oldstat);
        if (ret >= 0) {
            break;
        }
        printf(" sceSdBlockTrans ERROR!!\n");
    }

    BgmMode &= 0xfff;
    BgmMode |= 0x1000;
    return ret;
}

void _BgmStop(void) {
    int ret         = 0;
    int BgmMode_tmp = BgmMode;

    BgmMode &= 0xfff;
    BgmMode &= ~0x800;

    if ((BgmMode_tmp &= 0x1000) != 0) {
        while (1) {
            CpuSuspendIntr(&oldstat);
            ret = sceSdBlockTrans(1, 0x2, NULL, 0);
            CpuResumeIntr(oldstat);
            if (ret >= 0) {
                break;
            }
            printf(" sceSdBlockTrans ERROR!!\n");
        }
    }

    gBgmIntr = 0;
    wavep2.readBackFlag = FALSE;
    return; /* Explicit return needed to match. */
}

void BgmStop(unsigned int vol) {
    int ret         = 0;
    int BgmMode_tmp = BgmMode;

    BgmSetVolumeDirect(0);

    BgmMode &= 0xfff;
    BgmMode &= ~0x800;

    if ((BgmMode_tmp &= 0x1000) != 0) {
        while (1) {
            CpuSuspendIntr(&oldstat);
            ret = sceSdBlockTrans(1, 0x2, NULL, 0);
            CpuResumeIntr(oldstat);
            if (ret >= 0) {
                break;
            }
            printf(" sceSdBlockTrans ERROR!!\n");
        }
    }

    gBgmIntr = 0;
    wavep2.readBackFlag = FALSE;

    if (bgmPlayReadMode == RDMODE_CD) {
        TerminateThread(gThid);
        StartThread(gThid, NULL);
    }

    return; /* Explicit return needed to match. */
}

void BgmSetVolume(unsigned int vol) {
    BgmVolumeSet = 1;
    BgmVolume = vol;
    return; /* Explicit return needed to match. */
}

void BgmSetMode(u_int maxChan) {
    CpuSuspendIntr(&oldstat);

    wavep2.Channel = maxChan;

    wavep2.ReqChan[0] = 0;
    wavep2.ReqChan[1] = 1;

    wavep2.TransPos = 0;
    wavep2.StartTrPos = 0;

    /* Unnnecessary div+mult but required to match. */
    wavep2.TransMax = ((ReadBuffSize / 512) / maxChan) / 2 * 2;
    wavep2.Tr1Size = maxChan * 512;

    CpuResumeIntr(oldstat);
}

unsigned int BgmGetMode(void) {
    return BgmMode;
}

int GetKoubaisuu(int a, int b) {
    int common;
    int i;

    if (a < b) {
        common = a;
        a = b;
        b = common;
    }

    if ((a - b) == 1) {
        return (a * b);
    }

    for (i = 1; i <= b; i++) {
        common = a * i;
        if ((common % b) == 0) {
            break;
        }
    }

    return common;
}

int BgmSeek(unsigned int ofs) {
    int ofs_size;
    int ret;
    int next_ofs;
    int line_cnt;

    BgmSetVolumeDirect(0);

    line_cnt = GetKoubaisuu(4, wavep2.Tr1Size / 512);
    line_cnt /= (wavep2.Tr1Size / 512);

    next_ofs = ofs % line_cnt;
    ofs -= next_ofs;
    ofs_size = (ofs * wavep2.Tr1Size) / 2048;

    wavep2.StartTrPos = ofs;
    wavep2.TransPos = 0;

    if (bgmPlayReadMode == RDMODE_CD) {
        do {
            ret = sceCdStSeekF(fpCd.lsn + ofs_size);
        } while (ret == 0);
    } else {
        lseek(fp_pc, (ofs_size * 2048), SEEK_SET);
    }

    wavep2.ofs = ofs_size;
    wavep2.pos = wavep2.ofs;

    ReadOutCnt = ((wavep2.pos * 2048) / wavep2.Tr1Size) + wavep2.TransPos;

    gBgmIntr = 0;
    wavep2.readBackFlag = FALSE;
    BgmMode |= 0x800;
    return ret;
}

int BgmSeekFLoc(sceCdlFILE *fpLoc) {
    u_char *filename;
    int     ret;

    filename = (u_char*)fpLoc;

    if (filename[0] == 'h' && filename[1] == 'o' &&
        filename[2] == 's' && filename[3] == 't') {
        bgmPlayReadMode = RDMODE_PC;
    } else {
        bgmPlayReadMode = RDMODE_CD;
    }

    if (bgmPlayReadMode != RDMODE_CD) {
        if (fp_pc >= 0) {
            close(fp_pc);
        }

        fp_pc = open(filename, O_RDONLY);

        if (fp_pc < 0) {
            ret = 0;
        } else {
            ret = 1;
            fpCd.size = lseek(fp_pc, 0, SEEK_END);
            fpCd.lsn = 0;
        }
    } else {
        fpCd = *fpLoc;
    }

    gBgmIntrTime = 0;
    gBgmIntr = 0;
    ReadOutCnt = 0;

    wavep2.size = fpCd.size / 2048;
    wavep2.ofs = 0;
    wavep2.pos = 0;
    wavep2.StartTrPos = 0;
    wavep2.TransPos = 0;
    wavep2.readBackFlag = 0;

    BgmMode |= 0x800;
    return 1;
}

void BgmSetChannel(u_int chan) {
    wavep2.ReqChan[0] = chan >> 16;
    wavep2.ReqChan[1] = chan;
}

void BgmSetTrPoint(u_int trpos) {
    wavep2.TransEEAdrs = trpos;
}

int BgmGetTime(void) {
    u_int ret;
    u_int now_cnt;
    u_int gBgmIntrTmp;

    while (1) {
        gBgmIntrTime = 0;

        ret = sceSdBlockTransStatus(1, 0);
        now_cnt = ReadOutCnt;

        gBgmIntrTmp = gBgmIntr;
        if (gBgmIntrTime == 0) {
            break;
        }
    }

    if (BgmMode & 0x1000) {
        ret &= 0xffffff;
        if (ret >= sbuf.buf_pos[1]) {
            ret -= sbuf.buf_pos[1];
        } else {
            ret -= sbuf.buf_pos[0];
        }
    } else {
        ret = 0;
    }

    now_cnt += (sbuf.TrackSize / 2) * gBgmIntrTmp;
    return (now_cnt + (ret / 1024));
}

int BgmGetTSample() {
    int fsize = (wavep2.size * 2048);
    return fsize / wavep2.Tr1Size;
}

int BgmGetCdErrCode(void) {
    return CdErrCode;
}

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _BgmPlay);
