#ifndef MENUFONT_H
#define MENUFONT_H

#include "common.h"

#include <eetypes.h>

#include "main/subt.h"
#include "menu/pksprite.h"

enum {
    SUBTN_ETC_CODE,
    SUBTN_KANJI_CODE,
    SUBTN_PADSYM_CODE,
    SUBTN_END
};

typedef struct { // 0x14
    /* 0x00 */ u_short u;
    /* 0x02 */ u_short v;
    /* 0x04 */ u_short w;
    /* 0x06 */ u_short h;
    /* 0x08 */ char adjx;
    /* 0x09 */ char adjy;
    /* 0x0a */ MCODE_DAT apat;
} MCODE_DAT_ANIME;

typedef struct { // 0x16
    /* 0x00 */ u_short code;
    /* 0x02 */ u_short u;
    /* 0x04 */ u_short v;
    /* 0x06 */ u_short w;
    /* 0x08 */ u_short h;
    /* 0x0a */ char adjx;
    /* 0x0b */ char adjy;
    /* 0x0c */ MCODE_DAT apat;
} MCODE_KANJI_ANIME;

typedef struct { // 0x8
    /* 0x0 */ int flg;
    /* 0x4 */ MCODE_DAT *pmcode;
} MCODE_CHAR;

typedef struct { // 0x10
    /* 0x0 */ u_long tex0;
    /* 0x8 */ u_int abgr;
    /* 0xc */ u_int pad;
} MNFONT_INFO;

void MenuFont_ASC2EUC(char *des, char *src);

void MENUSubtSetKanji(void *kanji_data_top);
void MENUSubt_PadFontSw(int flg);
void MENUSubt_PadFontArrowSet(int flg);

int MENUSubtGetLine(u_char *str, int lflg);

void MENUSubtPut(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str, int lflg);
void MENUFontPutL(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str);
void MENUFontPutS(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str);
void MENUFontPutR(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str, float dw);

#endif