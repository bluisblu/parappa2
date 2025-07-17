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

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", TransBufSet);

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

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetMasterVolume);

void BgmCdInit(int mode) {
    sceCdInit(0);
    sceCdMmode(mode);
    sceCdStInit(256, 16, (u_int)ring_buf);
    sceCdDiskReady(0);
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

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmQuit);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmOpen);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmOpenFLoc);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmClose);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", TransBufSet_SUB);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmPreLoad);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _PreLoadBack);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmPreLoadBack);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmReadBuffFull);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmStart);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _BgmStop);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmStop);

void BgmSetVolume(unsigned int vol) {
    BgmVolumeSet = 1;
    BgmVolume = vol;
    return; /* Explicit return needed to match. */
}

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSetMode);

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
    wavep2.readBackFlag = 0;
    BgmMode |= 0x800;
    return ret;
}

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmSeekFLoc);

void BgmSetChannel(u_int chan) {
    wavep2.ReqChan[0] = chan >> 16;
    wavep2.ReqChan[1] = chan;
}

void BgmSetTrPoint(u_int trpos) {
    wavep2.TransEEAdrs = trpos;
}

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", BgmGetTime);

int BgmGetTSample() {
    int fsize = (wavep2.size * 2048);
    return fsize / wavep2.Tr1Size;
}

int BgmGetCdErrCode(void) {
    return CdErrCode;
}

IOP_INCLUDE_RODATA("wp2cd/nonmatchings/iop/bgm_play", D_00003A50);

IOP_INCLUDE_ASM("wp2cd/nonmatchings/iop/bgm_play", _BgmPlay);
