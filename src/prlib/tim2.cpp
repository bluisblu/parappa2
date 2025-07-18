#include "tim2.h"

#include <eestruct.h>
#include <eetypes.h>
#include <libgraph.h>

static void Tim2LoadTexture(int psm, u_int tbp, int tbw, int w, int h, u_long128 *pImage);

PR_EXTERN
int Tim2CheckFileHeader() {
    return 1;
}

PR_EXTERN
TIM2_PICTUREHEADER* Tim2GetPictureHeader(void *pTim2, int imgno) {
    TIM2_FILEHEADER    *pFileHdr = (TIM2_FILEHEADER*)pTim2;
    TIM2_PICTUREHEADER *pPictHdr;

    if (imgno >= pFileHdr->Pictures) {
        return NULL;
    }

    if (pFileHdr->FormatId == 0x00) {
        pPictHdr = (TIM2_PICTUREHEADER*)((char*)pTim2 + sizeof(TIM2_FILEHEADER));
    } else {
        pPictHdr = (TIM2_PICTUREHEADER*)((char*)pTim2 + 0x80);
    }

    for (int i = 0; i < imgno; i++) {
        pPictHdr = (TIM2_PICTUREHEADER*)((char*)pPictHdr + pPictHdr->TotalSize);
    }

    return pPictHdr;
}

PR_EXTERN
int Tim2GetMipMapPictureSize(TIM2_PICTUREHEADER *ph, int mipmap, int *pWidth, int *pHeight) {
    int w, h, n;

    w = ph->ImageWidth  >> mipmap;
    h = ph->ImageHeight >> mipmap;

    if (pWidth != NULL) {
        *pWidth = w;
    }
    if (pHeight != NULL) {
        *pHeight = h;
    }

    n = w * h;

    switch (ph->ImageType) {
    case TIM2_RGB16:
        n *= 2;
        break;
    case TIM2_RGB24:
        n *= 3;
        break;
    case TIM2_RGB32:
        n *= 4;
        break;
    case TIM2_IDTEX4:
        n /= 2;
        break;
    case TIM2_IDTEX8:
        break;
    }

    n = (n + 15) & ~15;
    return n;
}

PR_EXTERN
TIM2_MIPMAPHEADER* Tim2GetMipMapHeader(TIM2_PICTUREHEADER *ph, int *pSize) {
    TIM2_MIPMAPHEADER *pMmHdr;
    extern char mmsize[8];
    
    if (ph->MipMapTextures > 1) {
        pMmHdr = (TIM2_MIPMAPHEADER*)((char*)ph + sizeof(TIM2_PICTUREHEADER));
    } else {
        pMmHdr = NULL;
    }

    if (pSize != NULL) {
        *pSize = mmsize[ph->MipMapTextures];
    }

    return pMmHdr;
}

PR_EXTERN
void* Tim2GetImage(TIM2_PICTUREHEADER *ph, int mipmap) {
    void *pImage;

    if (mipmap >= ph->MipMapTextures) {
        return NULL;
    }

    pImage = (void*)((char*)ph + ph->HeaderSize);
    if (ph->MipMapTextures == 1) {
        return pImage;
    }
    
    TIM2_MIPMAPHEADER *pm = (TIM2_MIPMAPHEADER*)((char*)ph + sizeof(TIM2_PICTUREHEADER));
    for (int i = 0; i < mipmap; i++) {
        pImage = (void*)((char*)pImage + pm->MMImageSize[i]);
    }

    return pImage;
}

PR_EXTERN
void* Tim2GetClut(TIM2_PICTUREHEADER *ph) {
    void *pClut;

    if (ph->ClutColors == 0) {
        pClut = NULL;
    } else {
        pClut = (void*)((char*)ph + ph->HeaderSize + ph->ImageSize);
    }

    return pClut;
}

PR_EXTERN
u_int Tim2GetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no) {
    u_char *pClut = (u_char*)Tim2GetClut(ph);
    int n;
    u_char r, g, b, a;

    if (pClut == NULL) {
        return 0;
    }

    switch (ph->ImageType) {
    case TIM2_IDTEX4:
        n = clut * 16 + no;
        break;
    case TIM2_IDTEX8:
        n = clut * 256 + no;
        break;
    default:
        return 0;
    }

    if (n > ph->ClutColors) {
        return 0;
    }

    switch((ph->ClutType << 8) | ph->ImageType) {
    case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4):
    case (( TIM2_RGB16         << 8) | TIM2_IDTEX8):
    case (( TIM2_RGB24         << 8) | TIM2_IDTEX8):
    case (( TIM2_RGB32         << 8) | TIM2_IDTEX8):
        if ((n & 31) >= 8) {
            if ((n & 31) < 16) {
                n += 8;
            } else if ((n & 31) < 24) {
                n -= 8;
            }
        }

        break;
    }

    switch (ph->ClutType & 0x3f) {
    case TIM2_RGB16:
        r = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) << 3) & 0xf8);
        g = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 2) & 0xf8);
        b = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 7) & 0xf8);
        a = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 8) & 0x80);
        break;
    case TIM2_RGB24:
        r = pClut[n * 3];
        g = pClut[n * 3 + 1];
        b = pClut[n * 3 + 2];
        a = 128;
        break;
    case TIM2_RGB32:
        r = pClut[n * 4];
        g = pClut[n * 4 + 1];
        b = pClut[n * 4 + 2];
        a = pClut[n * 4 + 3];
        break;
    default:
        r = 0;
        g = 0;
        b = 0;
        a = 0;
        break;
    }
    
    return ((a << 24) | (b << 16) | (g << 8) | r);
}

PR_EXTERN
u_int Tim2SetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no, u_int newcolor) {
    u_char *pClut = (u_char*)Tim2GetClut(ph);

    if (pClut == NULL) {
        return 0;
    }

    /* Calculate the index of the color. */
    int n;
    switch (ph->ImageType) {
    case TIM2_IDTEX4:
        n = clut * 16 + no;
        break;
    case TIM2_IDTEX8:
        n = clut * 256 + no;
        break;
    default:
        return 0;
    }

    if (n > ph->ClutColors) {
        return 0; /* If assumed CLUT set, index color doesn't exist. */
    }
    
    switch((ph->ClutType << 8) | ph->ImageType) {
    case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4):
    case (( TIM2_RGB16         << 8) | TIM2_IDTEX8):
    case (( TIM2_RGB24         << 8) | TIM2_IDTEX8):
    case (( TIM2_RGB32         << 8) | TIM2_IDTEX8):
        if ((n & 31) >= 8) {
            if ((n & 31) < 16) {
                n += 8;
            } else if ((n & 31) < 24) {
                n -= 8;
            }
        }

        break;
    }

    /* Get color data according to the CLUT pixel format. */
    u_char r, g, b, a;
    switch (ph->ClutType & 0x3f) {
    case TIM2_RGB16: /* 16 bits color */
        u_char rr, gg, bb, aa;
        r  = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) << 3) & 0xf8);
        g  = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 2) & 0xf8);
        b  = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 7) & 0xf8);
        a  = (u_char)((((pClut[n * 2 + 1] << 8) | pClut[n * 2]) >> 8) & 0x80);

        rr = (u_char)((newcolor >> 3)   & 0x1f);
        gg = (u_char)((newcolor >> 11)  & 0x1f);
        bb = (u_char)((newcolor >> 19)  & 0x1f);
        aa = (u_char)((newcolor >> 31)  & 1);

        pClut[n * 2]     = (u_char)((((aa << 15) | (bb << 10) | (gg << 5) | rr))      & 0xff);
        pClut[n * 2 + 1] = (u_char)((((aa << 15) | (bb << 10) | (gg << 5) | rr) >> 8) & 0xff);
        break;
    case TIM2_RGB24: /* 24 bits color */
        r = pClut[n * 3];
        g = pClut[n * 3 + 1];
        b = pClut[n * 3 + 2];
        a = 128;

        pClut[n * 3]     = (u_char)((newcolor)       & 0xff);
        pClut[n * 3 + 1] = (u_char)((newcolor >> 8)  & 0xff);
        pClut[n * 3 + 2] = (u_char)((newcolor >> 16) & 0xff);
        break;
    case TIM2_RGB32: /* 32 bits color */
        r = pClut[n * 4];
        g = pClut[n * 4 + 1];
        b = pClut[n * 4 + 2];
        a = pClut[n * 4 + 3];

        pClut[n * 4]     = (u_char)((newcolor)       & 0xff);
        pClut[n * 4 + 1] = (u_char)((newcolor >> 8)  & 0xff);
        pClut[n * 4 + 2] = (u_char)((newcolor >> 16) & 0xff);
        pClut[n * 4 + 3] = (u_char)((newcolor >> 24) & 0xff);
        break;
    default: /* Illegal pixel format */
        r = 0;
        g = 0;
        b = 0;
        a = 0;
        break;
    }

   return ((a << 24) | (b << 16) | (g << 8) | r);
}

PR_EXTERN
u_int Tim2GetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y) {
    u_char *pImage = (u_char*)Tim2GetImage(ph, mipmap);

    if (pImage == NULL) {
        return 0;
    }

    int w, h;
    Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);

    if (x > w || y > h) {
        return 0; /* Illegal tex. coordinates. */
    }

    int t = y * w + x;
    switch (ph->ImageType) {
    case TIM2_RGB16:
        return ((pImage[t * 2 + 1] << 8) | pImage[t * 2]);
    case TIM2_RGB24:
        return ((pImage[t * 3 + 2] << 16) | (pImage[t * 3 + 1] << 8) | pImage[t * 3]);
    case TIM2_RGB32:
        return ((pImage[t * 4 + 3] << 24) | (pImage[t * 4 + 2] << 16) | (pImage[t * 4 + 1] << 8) | pImage[t * 4]);
    case TIM2_IDTEX4:
        if (x & 1) {
            return pImage[t / 2] >> 4;
        } else {
            return pImage[t / 2] & 0x0f;
        }
    case TIM2_IDTEX8:
        return pImage[t];
    }

    return 0;
}

PR_EXTERN
u_int Tim2SetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y, u_int newtexel) {
    u_char *pImage = (u_char*)Tim2GetImage(ph, mipmap);

    if (pImage == NULL) {
        return 0;
    }

    int w, h;
    Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);

    if (x > w || y > h) {
        return 0; /* Illegal tex. coordinates. */
    }

    int t = y * w + x;
    u_int oldtexel;
    switch (ph->ImageType) {
    case TIM2_RGB16:
        oldtexel = (pImage[t * 2 + 1] << 8) | pImage[t * 2];
        pImage[t * 2]     = (u_char)((newtexel)       & 0xff);
        pImage[t * 2 + 1] = (u_char)((newtexel >> 8)  & 0xff);
        return oldtexel;
    case TIM2_RGB24:
        oldtexel = (pImage[t * 3 + 2] << 16) | (pImage[t * 3 + 1] << 8) | pImage[t * 3];
        pImage[t * 3]     = (u_char)((newtexel)       & 0xff);
        pImage[t * 3 + 1] = (u_char)((newtexel >> 8)  & 0xff);
        pImage[t * 3 + 2] = (u_char)((newtexel >> 16) & 0xff);
        return oldtexel;
    case TIM2_RGB32:
        oldtexel = (pImage[t * 4 + 3] << 24) | (pImage[t * 4 + 2] << 16) | (pImage[t * 4 + 1] << 8) | pImage[t * 4];
        pImage[t * 4]     = (u_char)((newtexel)       & 0xff);
        pImage[t * 4 + 1] = (u_char)((newtexel >> 8)  & 0xff);
        pImage[t * 4 + 2] = (u_char)((newtexel >> 16) & 0xff);
        pImage[t * 4 + 3] = (u_char)((newtexel >> 24) & 0xff);
        return oldtexel;
    case TIM2_IDTEX4:
        if (x & 1) {
            oldtexel = pImage[t / 2] >> 4;
            pImage[t / 2] = (u_char)((newtexel << 4) | oldtexel);
        } else {
            oldtexel = pImage[t / 2] & 0x0f;
            pImage[t / 2] = (u_char)((oldtexel << 4) | newtexel);
        }
        return oldtexel;
    case TIM2_IDTEX8:
        oldtexel = pImage[t];
        pImage[t] = (u_char)newtexel;
        return oldtexel;
    }

    return 0;
}

PR_EXTERN
u_int Tim2GetTextureColor(TIM2_PICTUREHEADER *ph, int mipmap, int clut, int x, int y) {
    if (Tim2GetImage(ph, mipmap) == NULL) {
        return NULL;
    }

    u_int t = Tim2GetTexel(ph, mipmap, (x >> mipmap), (y >> mipmap));
    switch (ph->ImageType) {
    case TIM2_RGB16:
        u_char r, g, b, a;
        r = (u_char)((t << 3) & 0xf8);
        g = (u_char)((t >> 7) & 0xf8);
        b = (u_char)((t >> 2) & 0xf8);
        a = (u_char)((t >> 8) & 0x80);
        return ((a << 24) | (g << 16) | (b << 8) | r);
    case TIM2_RGB24:
        return ((0x80 << 24) | (t & 0x00ffffff));
    case TIM2_RGB32:
        return t;
    case TIM2_IDTEX4:
    case TIM2_IDTEX8:
        return Tim2GetClutColor(ph, clut, t);
    }

    return 0;
}

PR_EXTERN
void Tim2LoadPicture(TIM2_PICTUREHEADER *ph) {
    Tim2LoadImage(ph);
    Tim2LoadClut(ph);
}

PR_EXTERN
void Tim2LoadImage(TIM2_PICTUREHEADER *ph) {
    TIM2_MIPMAPHEADER *pm = NULL;

    int psm = ((sceGsTex0*)&ph->GsTex0)->PSM;
    if (ph->MipMapTextures > 1) {
        pm = (TIM2_MIPMAPHEADER*)(ph + 1);
    }

    for (int i = 0; i < ph->MipMapTextures; i++) {
        int tbp, tbw;
        if (i == 0) {
            /* Mipmap level 0 */
            tbp = ((sceGsTex0*)&ph->GsTex0)->TBP0;
            tbw = ((sceGsTex0*)&ph->GsTex0)->TBW;
        } else if (i < 4) {
            /* Mipmap levels 1, 2 or 3 */
            tbp = (pm->GsMiptbp1 >> ((i - 1) * 0x14)) & 0x3fff;
            tbw = (pm->GsMiptbp1 >> ((i - 1) * 0x14 + 0xe)) & 0x3f;
        } else {
            /* Mipmap levels 4, 5 or 6 */
            tbp = (pm->GsMiptbp2 >> ((i - 4) * 0x14)) & 0x3fff;
            tbw = (pm->GsMiptbp2 >> ((i - 4) * 0x14 + 0xe)) & 0x3f;
        }

        int w, h;
        u_long128 *pImage = (u_long128*)Tim2GetImage(ph, i);
        Tim2GetMipMapPictureSize(ph, i, &w, &h);

        u_int npp = 0;
        switch (psm) {
        case SCE_GS_PSMCT32:
        case SCE_GS_PSMZ32:
            npp = (32 / 4);
            break;
        case SCE_GS_PSMCT24:
        case SCE_GS_PSMZ24:
            npp = (24 / 4);
            break;
        case SCE_GS_PSMCT16:
        case SCE_GS_PSMCT16S:
        case SCE_GS_PSMZ16:
        case SCE_GS_PSMZ16S:
            npp = (16 / 4);
            break;
        case SCE_GS_PSMT8:
        case SCE_GS_PSMT8H:
            npp = (8 / 4);
            break;
        case SCE_GS_PSMT4:
        case SCE_GS_PSMT4HL:
        case SCE_GS_PSMT4HH:
            npp = (4 / 4);
            break;
        }

        h = (((w * h * npp / 2 + 15) / 16) * 32 + (w * npp) - 1) / (w * npp);
        Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);
    }
}

PR_EXTERN
u_int Tim2LoadClut(TIM2_PICTUREHEADER *ph) {
    sceGsLoadImage li;

    if (ph->ClutType == TIM2_NONE) {
        return ph->ClutType;
    }

    ((sceGsTex0*)&ph->GsTex0)->CSM = 0; /* CLUT storage mode (always CSM1) */
    ((sceGsTex0*)&ph->GsTex0)->CSA = 0; /* CLUT entry offset (always 0) */
    
    u_int cpsm = ((sceGsTex0*)&ph->GsTex0)->CPSM;
    u_int cbp  = ((sceGsTex0*)&ph->GsTex0)->CBP;

    /* Calculate the top address of the CLUT data */
    u_long128 *pClut = (u_long128*)((char*)ph + ph->HeaderSize + ph->ImageSize);

    /* Transfer CLUT data into GS local memory.
     * CLUT data format, etc. are defined by CLUT type and texture type. */
    switch ((ph->ClutType << 8) | ph->ImageType) {
        case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 16bits, compound */
        case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 24bits, compound */
        case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 32bits, compound */
        case (( TIM2_RGB16         << 8) | TIM2_IDTEX8): /* 256 colors, CSM1, 16bits, compound */
        case (( TIM2_RGB24         << 8) | TIM2_IDTEX8): /* 256 colors, CSM1, 24bits, compound */
        case (( TIM2_RGB32         << 8) | TIM2_IDTEX8): /* 256 colors, CSM1, 32bits, compound */
            /* If 256 colors CLUT and storage mode is CSM1 or
             * 16 colors CLUT and storage mode is CSM1 and compounded flag is ON
             * pixels are already placed in the right place, so directly transferable. */
            Tim2LoadTexture(cpsm, cbp, 1, 16, (ph->ClutColors / 16), pClut);
            break;
        case (( TIM2_RGB16         << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 16bits, sequential */
        case (( TIM2_RGB24         << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 24bits, sequential */
        case (( TIM2_RGB32         << 8) | TIM2_IDTEX4): /* 16  colors, CSM1, 32bits, sequential */
        case (((TIM2_RGB16 | 0x80) << 8) | TIM2_IDTEX4): /* 16  colors, CSM2, 16bits, sequential */
        case (((TIM2_RGB24 | 0x80) << 8) | TIM2_IDTEX4): /* 16  colors, CSM2, 24bits, sequential */
        case (((TIM2_RGB32 | 0x80) << 8) | TIM2_IDTEX4): /* 16  colors, CSM2, 32bits, sequential */
        case (((TIM2_RGB16 | 0x80) << 8) | TIM2_IDTEX8): /* 256 colors, CSM2, 16bits, sequential */
        case (((TIM2_RGB24 | 0x80) << 8) | TIM2_IDTEX8): /* 256 colors, CSM2, 24bits, sequential */
        case (((TIM2_RGB32 | 0x80) << 8) | TIM2_IDTEX8): /* 256 colors, CSM2, 32bits, sequential */
            /* If 16 colors CLUT, storage mode is CSM1, and compounded flag is OFF or
             * if 16 colors CLUT and storage mode is CSM2 or
             * if 256 colors CLUT and storage mode is CSM2:
             *
             * Sequential placement (CSM2) is ineffective in performance, so compound
             * into CSM1, then transfer. */
            for (int i = 0; i < ph->ClutColors / 16; i++) {
                sceGsSetDefLoadImage(&li, cbp, 1, cpsm, (i & 1) * 8, (i >> 1) * 2, 8, 2);
                FlushCache(WRITEBACK_DCACHE);

                /* Transfer CLUT data to GS and wait for completion. */
                sceGsExecLoadImage(&li, pClut);
                sceGsSyncPath(0, 0);
                
                if ((ph->ClutType & 0x3f) == TIM2_RGB16) {
                    pClut = (u_long128*)((char*)pClut + 2 * 16); /* 16bit colors */
                } else if ((ph->ClutType & 0x3f) == TIM2_RGB24) {
                    pClut = (u_long128*)((char*)pClut + 3 * 16); /* 24bit colors */
                } else {
                    pClut = (u_long128*)((char*)pClut + 4 * 16); /* 32bit colors */
                }
            }

            break;
        default:
            break;
    }
}

static void Tim2LoadTexture(int psm, u_int tbp, int tbw, int w, int h, u_long128 *pImage) {
    int n, l;
    sceGsLoadImage li;
    u_long128 *p;

    switch (psm) {
    case SCE_GS_PSMZ32:
    case SCE_GS_PSMCT32:
        n = w * 4;
        break;
    case SCE_GS_PSMZ24:
    case SCE_GS_PSMCT24:
        n = w * 3;
        break;
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
        n = w * 2;
        break;
    case SCE_GS_PSMT8H:
    case SCE_GS_PSMT8:
        n = w;
        break;
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
    case SCE_GS_PSMT4:
        n = w / 2;
        break;
    default:
        return;
    }

    /* Not to exceed the max. DMA transfer limit of 512KB, split then transfer. */
    l = 32764 * 16 / n;
    for (int i = 0; i < h; i += l) {
        p = (u_long128*)((char*)pImage + n * i);
        if ((i + l) > h) {
            l = h - i;
        }

        /* Set up texture */
        sceGsSetDefLoadImage(&li, tbp, tbw, psm, 0, i, w, l);
        FlushCache(WRITEBACK_DCACHE);

        /* Transfer to VRAM */
        sceGsExecLoadImage(&li, p);
        sceGsSyncPath(0, 0);
    }
}
