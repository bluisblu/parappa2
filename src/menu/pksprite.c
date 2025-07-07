#include "menu/pksprite.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

/* .sbss */
extern sceGsScissor _PkDefSCISSOR;
extern sceGsZbuf _PkDefZBUFFER;

/* .sbss */
extern int _PkScrW;
extern int _PkScrH;

/* .lit4 */
float FLT_003990DC; /* cannot be defined as extern, needed for `rotcossin` */

static void _tsWorkEnd(TS_WORKMEM *emem) {
    if (emem->isAlloc && emem->top) {
        free(emem->top);

        emem->top = NULL;
        emem->size = 0;
    }
}

static u_int* _tsWorkInit(TS_WORKMEM *emem, u_int *buf, u_int size) {
    _tsWorkEnd(emem);

    if (buf == NULL) {
        size = (size / 16) * 16;
        if (size != 0) {
            buf = memalign(128, size);
        }
        
        emem->top = buf;
        emem->size = size;
        emem->isAlloc = 1;
    } else {
        emem->top = buf;
        emem->size = size;
        emem->isAlloc = 0;
    }
    
    return emem->top;
}

u_long128* TsInitUPacket(TsUSERPKT *pk, u_long128 *buf, u_int size) {
    u_int      top;
    TsUSERPKT *p;
    u_int      b0, b1;

    memset(pk, 0, sizeof(TsUSERPKT));
    p = pk;

    top = (u_int)_tsWorkInit(&p->mem, (u_int*)buf, size);
    if (top == NULL) {
        return NULL;
    }

    p->size = size / 16;

    p->pkt[0].PaketTop = top;
    p->pkt[1].PaketTop = top;

    b0 = b1 = PR_UNCACHED(p->pkt[p->idx].PaketTop);

    p->ptop = b0;
    p->btop = b1;
    return (u_long128*)top;
}

void TsEndUPacket(TsUSERPKT *pk) {
    TsUSERPKT *p = pk;

    _tsWorkEnd(&p->mem);

    p->pkt[0].PaketTop = NULL;
    p->pkt[1].PaketTop = NULL;

    p->size = 0;
    p->btop = NULL;
    p->ptop = NULL;
}

/* EUC-JP string: "★パケットSizeOver!!(User)(%x/%x)\n" */
#if 1
INCLUDE_ASM("menu/pksprite", TsDrawUPacket);
#else
void TsDrawUPacket(/* s1 17 */ TsUSERPKT *up)
{
    /*
        up:P397
        pk:r398=*375
        qwc:r22
        PktChan:r399=*252
        -------
        tp:r255
        top:r22
        ------
        qwc:r1
        addr:r29
        ------
        addr:r29
    */

    /* s2 */ TSPAKET *pk = &up->pkt[up->idx];
    /* s0 */ u_int    qwc = (up->ptop & ~0x20000000) - pk->PaketTop;
    /* s0 */ sceDmaChan *PktChan;
    /* a3 7 */ sceDmaTag *tp;
    /* a1 5 */ u_int top;
    /* s0 16 */ //int qwc;
    /* a1 5 */ //void *addr;
    /* a1 5 */ //void *addr;

    if (qwc != 0)
    {
        if (qwc > up->size)
        {
            printf("★パケットSizeOver!!(User)(%x/%x)\n", qwc);
        }
        //else
        {
            top = pk->PaketTop;
            tp  = &pk->tag[0];

            if (qwc < 0xfff0)
            {
                void* addr = sceDmaAddRef(&tp, qwc, &top);
            }
            else
            {
                // int qwc;
                void *addr;

                while (qwc >= 0xfff0)
                {
                    addr = sceDmaAddRef(&tp, 0xfff0, &top);
                    qwc -= 0xfff0;

                    top += 0xfff0;
                }

                addr = sceDmaAddRef(&tp, qwc, &top);
            }

            sceDmaAddEnd(&tp, 0, NULL);

            up->idx  = 1 - up->idx;
            up->btop = up->pkt[1 - up->idx].PaketTop | 0x20000000;
            up->ptop = up->pkt[1 - up->idx].PaketTop | 0x20000000;

            PktChan = sceDmaGetChan(SCE_DMA_GIF);
            FlushCache(WRITEBACK_DCACHE);

            sceDmaSync(PktChan, 0, 0);
            sceDmaSend(PktChan, pk);
        }
    }
}
#endif

void PkTEX0_Add(SPR_PKT pkt, u_long texreg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = texreg;
    ((u_long*)*pk)[3] = SCE_GS_TEX0_1;

    *pkt = (u_long128*)pk + 2;
}

void PkTEX1_Add(SPR_PKT pkt, u_long texreg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = texreg;
    ((u_long*)*pk)[3] = SCE_GS_TEX1_1;

    *pkt = (u_long128*)pk + 2;
}

void PkCLAMP_Add(SPR_PKT pkt, u_long texrp) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = texrp;
    ((u_long*)*pk)[3] = SCE_GS_CLAMP_1;

    *pkt = (u_long128*)pk + 2;
}

void PkALPHA_Add(SPR_PKT pkt, u_long alpreg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = alpreg;
    ((u_long*)*pk)[3] = SCE_GS_ALPHA_1;

    *pkt = (u_long128*)pk + 2;
}

void PkTEST_Add(SPR_PKT pkt, u_long testsw) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = testsw;
    ((u_long*)*pk)[3] = SCE_GS_TEST_1;

    *pkt = (u_long128*)pk + 2;
}

void PkSCISSOR_Add(SPR_PKT pkt, short x, short y, short w, short h) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;

    ((short*)*pk)[8]  = x;
    ((short*)*pk)[9]  = x + w - 1;
    ((short*)*pk)[10] = y;
    ((short*)*pk)[11] = y + h - 1;

    ((u_long*)*pk)[3] = SCE_GS_SCISSOR_1;

    *pkt = (u_long128*)pk + 2;
}

void PkDefSCISSOR_Add(SPR_PKT pkt) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = *(u_long*)&_PkDefSCISSOR;
    ((u_long*)*pk)[3] = SCE_GS_SCISSOR_1;

    *pkt = (u_long128*)pk + 2;
}

void PkOFFSET_Add(SPR_PKT pkt, int x, int y) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;

    ((short*)*pk)[8]  = x;
    ((short*)*pk)[10] = y;

    ((u_long*)*pk)[3] = SCE_GS_XYOFFSET_1;

    *pkt = (u_long128*)pk + 2;
}

void PkPABE_Add(SPR_PKT pkt, u_int flg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = flg;
    ((u_long*)*pk)[3] = SCE_GS_PABE;

    *pkt = (u_long128*)pk + 2;
}

void PkFBA_Add(SPR_PKT pkt, u_int flg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = flg;
    ((u_long*)*pk)[3] = SCE_GS_FBA_1;

    *pkt = (u_long128*)pk + 2;
}

void PkCCLAMP_Add(SPR_PKT pkt, u_int flg) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = flg;
    ((u_long*)*pk)[3] = SCE_GS_COLCLAMP;

    *pkt = (u_long128*)pk + 2;
}

static void PkDefReg_Add(SPR_PKT pkt) {
    qword *pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 2);
    ((u_long*)*pk)[1] = 0 |
        SCE_GIF_PACKED_AD << (0 * 4) |
        SCE_GIF_PACKED_AD << (1 * 4);

    ((u_long*)*pk)[3] = SCE_GS_TEXFLUSH;

    ((u_long*)*pk)[4] = SCE_GS_SET_TEXA(0, 1, 128);
    ((u_long*)*pk)[5] = SCE_GS_TEXA;

    *pkt = (u_long128*)pk + 3;
}

void PkTEX0_SetAdd(SPR_PKT pkt, int vram, int w, int h, int isLinear) {
    qword *pk;
    int    n;
    int    tw, th;

    pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, SCE_GIF_PACKED, 3);
    ((u_long*)*pk)[1] = 0 |
        SCE_GIF_PACKED_AD << (0 * 4) |
        SCE_GIF_PACKED_AD << (1 * 4) |
        SCE_GIF_PACKED_AD << (2 * 4);

    ((u_long*)*pk)[3] = SCE_GS_TEXFLUSH;

    th = 0;
    tw = 0;

    for (n = 1; n < w; n <<= 1) {
        tw++;
    }
    for (n = 1; n < h; n <<= 1) {
        th++;
    }

    if (tw == 0) {
        tw = 1;
    }
    if (th == 0) {
        th = 1;
    }

    if (isLinear) {
        ((u_long*)*pk)[4] = SCE_GS_SET_TEX1_1(0, 0, 1, 1, 0, 0, 0);
    } else {
        ((u_long*)*pk)[4] = SCE_GS_SET_TEX1_1(0, 0, 0, 0, 0, 0, 0);
    }
    
    ((u_long*)*pk)[5] = SCE_GS_TEX1_1;

    /* Macros to set (a portion of) the TEX0 register as two words */
    #define GS_SET_TEX0_W0(tbp, tbw, psm, tw, th) \
        ((tbp)        | ((tbw) << 14) | \
        ((psm) << 20) | ((tw)  << 26) | \
        ((th)  << 30))
    #define GS_SET_TEX0_W1(th, tcc) \
        (((th) >> 2)  | ((tcc) << 2)) /* Set last two bits of TH, and TCC */

    ((u_int*)*pk)[12] = GS_SET_TEX0_W0(vram, (w + 63) / 64, 0, tw, th);
    ((u_int*)*pk)[13] = GS_SET_TEX0_W1(th, 1);
    ((u_long*)*pk)[7] = SCE_GS_TEX0_1;

    *pkt = (u_long128*)pk + 4;

    #undef GS_SET_TEX0_W0
    #undef GS_SET_TEX0_W1
}

u_int GetDToneColor(u_int sbgr, u_int dbgr, int ton) {
    u_char *pd, *ps;
    int     sc, dc;
    int     r, g, b, a;
    u_int   col;

    ps = (u_char*)&sbgr;
    pd = (u_char*)&dbgr;

    dc = pd[0]; sc = ps[0];
    r = sc + (((dc - sc) * ton) >> 8);
    
    dc = pd[1]; sc = ps[1];
    g = sc + (((dc - sc) * ton) >> 8);

    dc = pd[2]; sc = ps[2];
    b = sc + (((dc - sc) * ton) >> 8);

    dc = pd[3]; sc = ps[3];
    a = sc + (((dc - sc) * ton) >> 8);

    asm volatile(
        "pcpyld  $2, %4, %3 \n\t"
        "pcpyld  $4, %2, %1 \n\t"
        "ppacw   $2, $2, $4 \n\t"
        "li      $8, 0xff   \n\t"
        "dsll32  $8, 0      \n\t"
        "ori     $8, 0xff   \n\t"
        "pcpyld  $8, $8, $8 \n\t"
        "pmaxw   $2, $2, $0 \n\t"
        "pminw   $2, $2, $8 \n\t"
        "ppach   $2, $0, $2 \n\t"
        "ppacb   %0, $0, $2 \n\t"
    : "=r"(col) : "r"(r), "r"(g), "r"(b), "r"(a)
    );

    return col;
}

#ifndef NON_MATCHING
INCLUDE_ASM("menu/pksprite", GetToneColorA);
#else
u_int GetToneColorA(/* 0x0(sp) */ u_int abgr, /* a1 5 */ int tona, /* a2 6 */ int tonb, /* a3 7 */ int tong, /* t0 8 */ int tonr) {
    u_char *pa;
    /* a0 4 */ u_int r;
    /* t1 9 */ u_int g;
    /* v1 3 */ u_int b;
    /* v0 2 */ u_int a;
    u_int col;

    pa = (u_char*)&abgr;

    r = pa[0] * tonr;
    g = pa[1] * tong;
    b = pa[2] * tonb;
    a = pa[3] * tona;

    r >>= 4;
    g >>= 4;
    b >>= 4;
    a >>= 4;

    asm volatile(
        "pcpyld  $2, %4, %3 \n\t"
        "pcpyld  $4, %2, %1 \n\t"
        "ppacw   $2, $2, $4 \n\t"
        "li      $8, 0xff   \n\t"
        "dsll32  $8, 0      \n\t"
        "ori     $8, 0xff   \n\t"
        "pcpyld  $8, $8, $8 \n\t"
        "pmaxw   $2, $2, $0 \n\t"
        "pminw   $2, $2, $8 \n\t"
        "ppach   $2, $0, $2 \n\t"
        "ppacb   %0, $0, $2 \n\t"
    : "=r"(col) : "r"(r), "r"(g), "r"(b), "r"(a)
    );

    return col;
}
#endif

INCLUDE_ASM("menu/pksprite", GetToneColorH);

void SetSprDefOfsXY(SPR_PRM *spr) {
    spr->ofsx = 2048.0f - _PkScrW * 0.5f;
    spr->ofsy = 2048.0f - _PkScrH * 0.5f;
}

void SetSprScreenXYWH(SPR_PRM *spr) {
    spr->sw = _PkScrW;
    spr->sh = _PkScrH;
    spr->px = spr->py = 0;
}

void PkSprPkt_SetDrawEnv(SPR_PKT pkt, SPR_PRM *spr, sceGsDrawEnv1 *pdenv) {
    qword *pk;

    if (pdenv == NULL) {
        return;
    }

    pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, SCE_GIF_PACKED, 3);
    ((u_long*)*pk)[1] = 0 |
        SCE_GIF_PACKED_AD << (0 * 4) |
        SCE_GIF_PACKED_AD << (1 * 4) |
        SCE_GIF_PACKED_AD << (2 * 4);

    ((u_long*)*pk)[3] = SCE_GS_TEXFLUSH;

    ((u_long*)*pk)[4] = *(u_long*)&pdenv->frame1;
    ((u_long*)*pk)[5] = SCE_GS_FRAME_1;

    ((u_long*)*pk)[6] = *(u_long*)&pdenv->zbuf1;
    ((u_long*)*pk)[7] = SCE_GS_ZBUF_1;

    *pkt = (u_long128*)pk + 4;

    _PkDefSCISSOR = pdenv->scissor1;
    _PkDefZBUFFER = pdenv->zbuf1;

    _PkScrW = _PkDefSCISSOR.SCAX1 - _PkDefSCISSOR.SCAX0 + 1;
    _PkScrH = _PkDefSCISSOR.SCAY1 - _PkDefSCISSOR.SCAY0 + 1;

    SetSprDefOfsXY(spr);
    PkDefSCISSOR_Add(pkt);
}

void PkZBUFMask_Add(SPR_PKT pkt, int bMsk) {
    qword *pk;

    if (_PkDefZBUFFER.ZBP == 0) {
        return;
    }

    pk = (qword*)*pkt;

    ((u_long*)*pk)[0] = SCE_GIF_SET_TAG(1, 1, 0, 0, SCE_GIF_PACKED, 1);
    ((u_long*)*pk)[1] = SCE_GIF_PACKED_AD;
    ((u_long*)*pk)[2] = *(u_long*)&_PkDefZBUFFER;
    ((u_long*)*pk)[3] = SCE_GS_ZBUF_1;

    *pkt = (u_long128*)pk + 2;

    ((sceGsZbuf*)*pk)[2].ZMSK = (bMsk != 0);
}

INCLUDE_ASM("menu/pksprite", PkSprPkt_SetTexVram);

INCLUDE_ASM("menu/pksprite", PkSprPkt_SetDefault);

INCLUDE_ASM("menu/pksprite", PkNSprite_Add);

INCLUDE_ASM("menu/pksprite", PkNSprite_Add2);

INCLUDE_ASM("menu/pksprite", PkNSprite_AddAdj);

INCLUDE_ASM("menu/pksprite", PkCRect_Add);

#if 1
INCLUDE_ASM("menu/pksprite", PkCGRect_Add);
void PkCGRect_Add(/* t4 12 */ SPR_PKT pk, /* a1 5 */ SPR_PRM *ppspr, /* a2 6 */ int flg);
#else
{
    /* a3 7 */ SprTagCG *sp = (SprTagCG*)*pk;

    *(u_long*)&sp->GifCord[0] = 0x9400000000008001;
    *(u_long*)&sp->GifCord[2] = 0x414141410;

    sp->prim  = 0x14c;
    sp->rgba0 = ppspr->rgba0;
    sp->rgba1 = ppspr->rgba1;
    sp->rgba2 = ppspr->rgba2;
    sp->rgba3 = ppspr->rgba3;

    asm __volatile__
    ("
        lqc2         vf01, 0(%0)
        lqc2         vf02, 0x10(%0)
        vitof0.xyzw  vf01, vf01
        vmul.zw      vf01, vf01, vf02
    " :: "r"(ppspr));

    if (flg & 2 && ppspr->zoom.isOn)
    {
        asm __volatile__
        ("
            lqc2         vf03, 0(%0)
            vmr32.xyzw   vf04, vf03
            vsub.xy      vf01, vf01, vf03
            vmr32.xyzw   vf04, vf04
            vmul.zw      vf01, vf01, vf03
            vmul.xy      vf01, vf01, vf04
            vadd.xy      vf01, vf01, vf03
        " :: "r"(&ppspr->zoom));
    }

    asm __volatile__
    ("
        vadd.xy vf01, vf01, vf02
        lw      $9, 0(%0)

        vmr32.xyzw   vf03, vf01
        vmr32.xyzw   vf03, vf03
        vadd.zw      vf01, vf01, vf03
        vftoi4.xyzw  vf01, vf01

        qmfc2        $8, vf01
        pexow        $11, $8
        ppach        $8, $0, $8
        ppach        $11, $0, $11
        pextlw       $10, $9, $8

        sd           $10, 0x20(%1) # a3
        pextlw       $10, $9, $11
        sd           $10, 0x30(%1)
        dsrl32       $10, $11, 0x0
        pextlw       $10, $9, $10
        sd           $10, 0x40(%1)
        dsrl32       $10, $8, 0x0
        pextlw       $10, $9, $10
        sd           $10, 0x50(%1) # a3
    " :: "r"(ppspr->zdepth), "r"(sp));

    *pk = sp->GifCord;
}
#endif

extern float S5432[4]; /* see SCE's libvu0 */

static void rotcossin(float rot) {
    asm volatile(
        "mtc1        $0, $f0             \n\t"
        "c.olt.s     %0, $f0             \n\t"
        "lwc1        $f0, FLT_003990DC   \n\t" /* li.s $f0, 1.57079637050628662109e0 */
        "bc1f        _RotCosSin_01       \n\t"
        "add.s       %0, $f0, %0         \n\t" /* rx += (pi/2) */
        "li          $7, 0x1             \n\t"
        "j           _RotCosSin_02       \n\t"

"_RotCosSin_01:                          \n\t"
        "sub.s       %0, $f0, %0         \n\t" /* rx = (pi/2)-rx */
        "move        $7, $0              \n\t"

"_RotCosSin_02:                          \n\t"
        "mfc1        $8, %0              \n\t"
        "qmtc2       $8, vf03            \n\t"

        "la          $8,   S5432         \n\t" /* Transfer coefficients of S5-S2 to VF05 */
        "lqc2        vf01, 0($8)         \n\t"

        "vmr32.w     vf03, vf03          \n\t" /* Copy VF03.x(v) to VF03.w */
        "vaddx.x     vf04, vf00, vf03    \n\t" /* Copy VF03.x(v) to VF04.x */
        "vmul.x      vf03, vf03, vf03    \n\t" /* Square VF03.x to v^2 */
        "vmulx.yzw   vf04, vf04, vf00    \n\t" /* VF04.yzw = 0 */
        "vmulw.xyzw  vf02, vf01, vf03    \n\t" /* Apply VF03.w(v) to S2-S5 */
        "vmulx.xyzw  vf02, vf02, vf03    \n\t" /* Multiply by VF03.x(v^2) */
        "vmulx.xyz   vf02, vf02, vf03    \n\t" /* Multiply by VF03.x(v^2) */
        "vaddw.x     vf04, vf04, vf02    \n\t" /* s += k2 */
        "vmulx.xy    vf02, vf02, vf03    \n\t" /* Multiply by VF03.x(v^2) */
        "vaddz.x     vf04, vf04, vf02    \n\t" /* s += z */
        "vmulx.x     vf02, vf02, vf03    \n\t" /* Multiply by VF03.x(v^2) */
        "vaddy.x     vf04, vf04, vf02    \n\t" /* s += y */
        "vaddx.x     vf04, vf04, vf02    \n\t" /* s += x (sin is over) */
        "vaddx.xy    vf04, vf19, vf04    \n\t" /* .xy = s (append) */
        "vmul.x      vf05, vf04, vf04    \n\t" /* VF05.x = s*s */
        "vsubx.w     vf05, vf00, vf05    \n\t" /* VF05.w = 1-(s*s) */

        "vsqrt       Q, vf05w            \n\t" /* Q = sqrt(1-s*s) (cos is over) */
        "vwaitq                          \n\t"
        
        "cfc2        $8, $vi22           \n\t"
        "qmtc2       $8, vf05            \n\t"

        "bne         $7, $0, _rcossin_01 \n\t"

        "vaddx.x     vf04, vf19, vf05    \n\t" /* VF04.x = s */
        "b           _rcossin_02         \n\t"

"_rcossin_01:                            \n\t"
        "vsubx.x     vf04, vf19, vf05    \n\t" /* VF04.x = s */
"_rcossin_02:                            \n\t"
    : : "f"(rot));
}

void _pkVU0RotMatrixZ(float rz) {
    asm volatile(
        "vsub.xyzw     vf19, vf0,  vf00"
    );

    rotcossin(rz);

    asm volatile(
        "vmove.xyzw    vf09, vf00       \n\t"
        "vmove.zw      vf06, vf19       \n\t"
        "vmove.zw      vf07, vf19       \n\t"
        "vsub.zw       vf04, vf04, vf04 \n\t"
        "vmr32.xyzw    vf08, vf09       \n\t"
        "vaddx.y       vf06, vf19, vf04 \n\t"
        "vaddy.x       vf06, vf19, vf04 \n\t"
        "vsubx.x       vf07, vf19, vf04 \n\t"
        "vaddy.y       vf07, vf19, vf04 \n\t"
    );
}

#if 1
INCLUDE_ASM("menu/pksprite", PkRSprite_Add);
#else
void PkRSprite_Add(/* s3 19 */ SPR_PKT pk, /* s0 16 */ SPR_PRM *ppspr, /* s2 18 */ int flg)
{
    /* s1 17 */ SprTagTFR *sp = (SprTagTFR*)*pk;

    _pkVU0RotMatrixZ(ppspr->rot);

    *(u_long*)&sp->GifCord[0] = 0xa400000000008001;
    *(u_long*)&sp->GifCord[2] = 0x4343434310;

    sp->prim = 0x154;
    sp->rgba = ppspr->rgba0;

    if (flg & 1)
    {
        asm __volatile__("lq $9, 0x20(%0)" :: "r"(ppspr)); // ppspr->ux => $t1
    }
    else
    {
        asm __volatile__("lq $9,  0x0(%0)" :: "r"(ppspr));
    }

    asm __volatile__
    ("
        # t0 -> $8
        # t1 -> $9
        # t2 -> $10

        lq     $8, 0x20(%0)     # ppspr->ux => $t0 ($8)
        li     $10, -8
        daddu  $9,  $0,  $8
        pcpyld $8,  $8,  $0
        paddw  $9,  $8,  $9
        psllw  $9,  $9,  0x4

        li     $8,  0x8         # t0 = 8
        pextlw $8,  $10, $8
        pextlw $8,  $8,  $8
        paddw  $8,  $9,  $8
        ppach  $9,  $0,  $8
        sd     $9,  0x20(%1)    # sp->uv0

        pexew  $10, $8
        ppach  $8,  $0, $10
        sd     $8,  0x30(%1)    # sp->uv1

        prot3w $10, $8
        sd     $10, 0x40(%1)    # sp->uv2

        prot3w $10, $9
        sd     $10, 0x50(%1)    # sp->uv3
    " :: "r"(ppspr), "r"(sp));

    asm __volatile__
    ("
        lqc2        vf01, 0x0(%0)
        lq          $8,   0x10(%0)

        vitof0.xyzw vf01, vf01

        qmtc2       $8, vf02
        pcpyud      $8, $8, $8

        qmtc2       $8, vf03
        lq          $8, 0x30(%0)
        pcpyld      $8, $8, $8
        qmtc2       $8, vf04
        vmul.xy     vf04, vf04, vf03
    " :: "r"(ppspr));

    if (flg & 2 && ppspr->zoom.isOn)
    {
        asm __volatile__
        ("
            lq       $8, 0x0(%0)

            qmtc2    $8, vf05
            pcpyud   $8, $8, $8

            qmtc2    $8, vf08
            vadda.xy ACC,  vf01, vf0
            vmadd.xy vf01, vf04, vf08
            vmul.zw  vf01, vf01, vf05
            vmul.zw  vf04, vf04, vf08
            vsub.xy  vf01, vf01, vf05
            vadda.xy ACC,  vf05, vf00
            vmadd.xy vf01, vf01, vf08
        " :: "r"(&ppspr->zoom));
    }

    asm __volatile__
    ("
        vadd.xy      vf01, vf01, vf04
        vadd.xy      vf08, vf01, vf02
        vmr32.xyzw   vf02, vf02
        vmr32.xyzw   vf04, vf04
        vmr32.xyzw   vf05, vf01
        vmr32.xyzw   vf02, vf02
        vmr32.xyzw   vf04, vf04
        vmr32.xyzw   vf05, vf05

        # t4 -> $12
        lw           $12,  0x0(%0)
        vmul.xy      vf06, vf06, vf02
        vmul.xy      vf07, vf07, vf02
        vsub.xy      vf09, vf00, vf04
        vsub.y       vf10, vf00, vf04
        vsub.x       vf10, vf05, vf04
        vsub.x       vf11, vf00, vf04
        vsub.y       vf11, vf05, vf04
        vsub.xy      vf12, vf05, vf04
        vmulax.xyzw  ACC,  vf06, vf09
        vmadday.xyzw ACC,  vf07, vf09
        vmaddw.xyzw  vf09, vf08, vf00
        vmulax.xyzw  ACC,  vf06, vf10
        vmadday.xyzw ACC,  vf07, vf10
        vmaddw.xyzw  vf10, vf08, vf00
        vftoi4.xyzw  vf09, vf09
        vmulax.xyzw  ACC,  vf06, vf11
        vmadday.xyzw ACC,  vf07, vf11
        vmaddw.xyzw  vf11, vf08, vf00
        vftoi4.xyzw  vf10, vf10

        qmtc2        $8,   vf09
        vmulax.xyzw  ACC,  vf06, vf12
        vmadday.xyzw ACC,  vf07, vf12
        vmaddw.xyzw  vf12, vf08, vf00
        vftoi4.xyzw  vf11, vf11

        qmfc2        $9,   vf10
        ppach        $8,  $0,  $8
        pextlw       $8,  $12, $8
        sd           $8,  0x28(%1)

        vftoi4.xyzw  vf12, vf12

        ppach        $8,  $0,  $9
        pextlw       $8,  $12, $8
        sd           $8,  0x38(%1)

        qmfc2        $10, vf11
        ppach        $8,  $0,  $10
        pextlw       $8,  $12, $8
        sd           $8,  0x48(%1)

        qmfc2        $11, vf12
        ppach        $8, $0,  $11
        pextlw       $8, $12, $8
        sd           $8, 0x58(%1)
    " :: "r"(ppspr->zdepth), "r"(sp));

    *pk = sp->GifCord;
}
#endif

INCLUDE_ASM("menu/pksprite", PkCLine2_Add);

INCLUDE_ASM("menu/pksprite", PkCLineS_AddStart);

INCLUDE_ASM("menu/pksprite", PkCLineS_AddNext);

INCLUDE_ASM("menu/pksprite", PkPolyF3_Add);

INCLUDE_ASM("menu/pksprite", PkPolyF4_Add);
void PkPolyF4_Add(/* t3 11 */ SPR_PKT pk, /* a1 5 */ SPR_PRM *ppspr, /* a2 6 */ int flg);

#if 1
INCLUDE_ASM("menu/pksprite", PkPolyFT4_Add);
void PkPolyFT4_Add(/* t3 11 */ SPR_PKT pk, /* a1 5 */ SPR_PRM *ppspr, /* a2 6 */ int flg);
#else
void PkPolyFT4_Add(/* t3 11 */ SPR_PKT pk, /* a1 5 */ SPR_PRM *ppspr, /* a2 6 */ int flg)
{
    /* a3 7 */ SprTagTFR *sp = (SprTagTFR*)*pk;

    *(u_long*)&sp->GifCord[0] = 0xa400000000008001;
    *(u_long*)&sp->GifCord[2] = 0x4343434310;

    sp->prim = 0x154;
    sp->rgba = ppspr->rgba0;

    asm __volatile__
    ("
        # t0 -> $8
        # t1 -> $9
        # t2 -> $10

        lq     $9, 0x20(%0)     # ppspr->ux => $t1 ($9)
        lq     $8, 0x20(%0)     # ppspr->ux => $t0 ($8)
        li     $10, -8
        daddu  $9,  $0,  $8
        pcpyld $8,  $8,  $0
        paddw  $9,  $8,  $9
        psllw  $9,  $9,  0x4

        li     $8,  0x8         # t0 = 8
        pextlw $8,  $10, $8
        pextlw $8,  $8,  $8
        paddw  $8,  $9,  $8
        ppach  $9,  $0,  $8
        sd     $9,  0x20(%1)    # sp->uv0

        pexew  $10, $8
        ppach  $8,  $0, $10
        sd     $8,  0x30(%1)    # sp->uv1

        prot3w $10, $8
        sd     $10, 0x40(%1)    # sp->uv2

        prot3w $10, $9
        sd     $10, 0x50(%1)    # sp->uv3
    " :: "r"(ppspr), "r"(sp));

    asm __volatile__
    ("
        lqc2        vf01, 0x50(%0)
        lqc2        vf02, 0x60(%0)
        lqc2        vf04, 0x10(%0)

        vmr32.xyzw  vf03, vf04
        vmr32.xyzw  vf03, vf03
        vadd.xy     vf03, vf00, vf04
    " :: "r"(ppspr));

    if (flg & 2 && ppspr->zoom.isOn)
    {
        asm __volatile__
        ("
            lq       $8, 0x0(%0)

            qmtc2    $8, vf05
            pcpyud   $8, $8, $8

            qmtc2    $8, vf08
            vadda.xy ACC,  vf01, vf0
            vmadd.xy vf01, vf04, vf08
            vmul.zw  vf01, vf01, vf05
            vmul.zw  vf04, vf04, vf08
            vsub.xy  vf01, vf01, vf05
            vadda.xy ACC,  vf05, vf00
            vmadd.xy vf01, vf01, vf08
        " :: "r"(&ppspr->zoom));
    }

    asm __volatile__
    ("
        vadd.xy      vf01, vf01, vf04
        vadd.xy      vf08, vf01, vf02
        vmr32.xyzw   vf02, vf02
        vmr32.xyzw   vf04, vf04
        vmr32.xyzw   vf05, vf01
        vmr32.xyzw   vf02, vf02
        vmr32.xyzw   vf04, vf04
        vmr32.xyzw   vf05, vf05

        # t4 -> $12
        lw           $12,  0x0(%0)
        vmul.xy      vf06, vf06, vf02
        vmul.xy      vf07, vf07, vf02
        vsub.xy      vf09, vf00, vf04
        vsub.y       vf10, vf00, vf04
        vsub.x       vf10, vf05, vf04
        vsub.x       vf11, vf00, vf04
        vsub.y       vf11, vf05, vf04
        vsub.xy      vf12, vf05, vf04
        vmulax.xyzw  ACC,  vf06, vf09
        vmadday.xyzw ACC,  vf07, vf09
        vmaddw.xyzw  vf09, vf08, vf00
        vmulax.xyzw  ACC,  vf06, vf10
        vmadday.xyzw ACC,  vf07, vf10
        vmaddw.xyzw  vf10, vf08, vf00
        vftoi4.xyzw  vf09, vf09
        vmulax.xyzw  ACC,  vf06, vf11
        vmadday.xyzw ACC,  vf07, vf11
        vmaddw.xyzw  vf11, vf08, vf00
        vftoi4.xyzw  vf10, vf10

        qmtc2        $8,   vf09
        vmulax.xyzw  ACC,  vf06, vf12
        vmadday.xyzw ACC,  vf07, vf12
        vmaddw.xyzw  vf12, vf08, vf00
        vftoi4.xyzw  vf11, vf11

        qmfc2        $9,   vf10
        ppach        $8,  $0,  $8
        pextlw       $8,  $12, $8
        sd           $8,  0x28(%1)

        vftoi4.xyzw  vf12, vf12

        ppach        $8,  $0,  $9
        pextlw       $8,  $12, $8
        sd           $8,  0x38(%1)

        qmfc2        $10, vf11
        ppach        $8,  $0,  $10
        pextlw       $8,  $12, $8
        sd           $8,  0x48(%1)

        qmfc2        $11, vf12
        ppach        $8, $0,  $11
        pextlw       $8, $12, $8
        sd           $8, 0x58(%1)
    " :: "r"(ppspr->zdepth), "r"(sp));

    // **pk = *(u_long128*)&sp->GifCord;
    *pk = sp->GifCord;
}
#endif

PKMESH* PkMesh_Create(int w, int h) {
    PKMESH *pmesh = malloc(sizeof(PKMESH));

    if (pmesh == NULL) {
        return NULL;
    }

    memset(pmesh, 0, sizeof(PKMESH));
    pmesh->pmspt = malloc(sizeof(PKMSPT) * (w + 1) * (h + 1));

    if (pmesh->pmspt == NULL) {
        free(NULL);
        return NULL;
    } else {
        pmesh->mw = w;
        pmesh->mh = h;
    }

    return pmesh;
}

void PkMesh_Delete(PKMESH *mesh) {
    if (mesh == NULL) {
        return;
    }

    if (mesh->pmspt != NULL) {
        free(mesh->pmspt);
    }

    free(mesh);
}

void PkMesh_SetXYWH(PKMESH *mesh, float px0, float py0, float sw, float sh) {
    PKMSPT *pt;
    int     x, y;
    float   fmw, fmh;
    float   py;

    pt = mesh->pmspt;

    mesh->px = px0;
    mesh->py = py0;
    
    mesh->sw = sw;
    mesh->sh = sh;

    fmw = 1.0f / mesh->mw;
    fmh = 1.0f / mesh->mh;

    for (y = 0; y < (mesh->mh + 1); y++) {
        py = py0 + ((y * sh) * fmh);

        for (x = 0; x < (mesh->mw + 1); x++, pt++) {
            pt->x = px0 + ((x * sw) * fmw);
            pt->y = py;

            pt->ofsx = pt->ofsy = 0.0f;
        }
    }
}

void PkMesh_SetUVWH(PKMESH *mesh, float ux0, float uy0, float uw, float uh) {
    PKMSPT *pt;
    int     x, y;
    float   fmw, fmh;
    int     uy;

    pt = mesh->pmspt;

    fmw = 1.0f / mesh->mw;
    fmh = 1.0f / mesh->mh;

    for (y = 0; y < (mesh->mh + 1); y++) {
        uy = uy0 + ((y * uh) * fmh);

        for (x = 0; x < (mesh->mw + 1); x++, pt++) {
            pt->u = ux0 + ((x * uw) * fmw);
            pt->v = uy;
        }
    }
}

void PkCMesh_Add(SPR_PKT pk, SPR_PRM *spr, PKMESH *mesh) {
    int     cidx, lidx;
    int     x, y;
    PKMSPT *pt;

    cidx = 0;
    lidx = mesh->mw + 1;

    for (y = 0; y < mesh->mh; y++) {
        pt = &mesh->pmspt[cidx];

        for (x = 0; x < mesh->mw; x++, pt++) {
            spr->px0 = pt[0].x + pt[0].ofsx;
            spr->py0 = pt[0].y + pt[0].ofsy;

            spr->px1 = pt[1].x + pt[1].ofsx;
            spr->py1 = pt[1].y + pt[1].ofsy;

            spr->px2 = pt[lidx].x + pt[lidx].ofsx;
            spr->py2 = pt[lidx].y + pt[lidx].ofsy;

            spr->px3 = pt[lidx + 1].x + pt[lidx + 1].ofsx;
            spr->py3 = pt[lidx + 1].y + pt[lidx + 1].ofsy;

            PkPolyF4_Add(pk, spr, 0);
        }

        cidx += lidx;
    }
}

void PkFTMesh_Add(SPR_PKT pk, SPR_PRM *spr, PKMESH *mesh) {
    int     cidx, lidx;
    int     x, y;
    PKMSPT *pt;

    cidx = 0;
    lidx = mesh->mw + 1;

    for (y = 0; y < mesh->mh; y++) {
        pt = &mesh->pmspt[cidx];

        for (x = 0; x < mesh->mw; x++, pt++) {
            spr->ux  = pt[0].u;
            spr->uy  = pt[0].v;

            spr->uw  = pt[1].u    - pt[0].u;
            spr->uh  = pt[lidx].v - pt[0].v;

            spr->px0 = pt[0].x + pt[0].ofsx;
            spr->py0 = pt[0].y + pt[0].ofsy;

            spr->px1 = pt[1].x + pt[1].ofsx;
            spr->py1 = pt[1].y + pt[1].ofsy;

            spr->px2 = pt[lidx].x + pt[lidx].ofsx;
            spr->py2 = pt[lidx].y + pt[lidx].ofsy;

            spr->px3 = pt[lidx + 1].x + pt[lidx + 1].ofsx;
            spr->py3 = pt[lidx + 1].y + pt[lidx + 1].ofsy;

            PkPolyFT4_Add(pk, spr, 2);
        }

        cidx += lidx;
    }
}

void PkMesh_SetHLinOfs(PKMESH *mesh, int no, float x, float y) {
    PKMSPT *pt;
    int     i;

    if (mesh->mh < no) {
        return;
    }
    
    pt = &mesh->pmspt[(mesh->mw + 1) * no];
    for (i = 0; i < mesh->mw + 1; i++, pt++) {
        pt->ofsx = x;
        pt->ofsy = y;
    }
}

void PkMesh_SetVLinOfs(PKMESH *mesh, int no, float x, float y) {
    PKMSPT *pt;
    int     i;

    if (mesh->mw < no) {
        return;
    }

    pt = &mesh->pmspt[no];
    for (i = 0; i < mesh->mh + 1; i++, pt += (mesh->mw + 1)) {
        pt->ofsx = x;
        pt->ofsy = y;
    }
}

void PkMesh_SetHLinOfsLRX(PKMESH *mesh, int no, float lofsx, float rofsx) {
    PKMSPT *pt;
    int     i;
    float   x, dx;

    x  = lofsx;
    dx = (rofsx - lofsx) / mesh->mw;

    if (mesh->mh < no) {
        return;
    }

    pt = &mesh->pmspt[(mesh->mw + 1) * no];
    for (i = 0; i < mesh->mw + 1; i++, pt++) {
        pt->ofsx = x;
        x += dx;
    }
}

void PkMesh_SetVLinOfsUDY(PKMESH *mesh, int no, float uofsy, float dofsy) {
    PKMSPT *pt;
    int     i;
    float   y, dy;

    y  = uofsy;
    dy = (uofsy - dofsy) / mesh->mh;

    if (mesh->mw < no) {
        return;
    }

    pt = &mesh->pmspt[no];
    for (i = 0; i < mesh->mh + 1; i++, pt += (mesh->mw + 1)) {
        pt->ofsy = y;
        y += dy;
    }
}
