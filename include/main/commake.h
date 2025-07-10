#ifndef COMMAKE_H
#define COMMAKE_H

#include "common.h"

#include "main/scrctrl.h"

#include <eetypes.h>

typedef struct { // 0x4
    /* 0x0 */ u_short keyId;
    /* 0x2 */ short timeOfs;
} CM_STR;

typedef struct { // 0x1c4
    /* 0x000 */ CM_STR cm_str_mt[32];
    /* 0x080 */ CM_STR cm_str_now[32];
    /* 0x100 */ CM_STR cm_str_make[32];
    /* 0x180 */ int keyKind;
    /* 0x184 */ int keyKindNum;
    /* 0x188 */ int keyCnt_mt[7];
    /* 0x1a4 */ int keyCnt_now[7];
    /* 0x1c0 */ int maxBox;
} CM_STR_CTRL;

typedef struct { // 0x8
    /* 0x0 */ int time;
    /* 0x4 */ short KeyIndex;
} COMMAKE_STR;

void    comMakeSubYure(CM_STR *cms_pp, int cnt, int min, int max);
void    comMakeSubYureReset(CM_STR *cms_pp, int cnt);
void    comMakeSubChangeKey(CM_STR *cms_pp, int cnt, int motoKey, int sakiKey, int missCnt);
void    comMakeSubSwapKey(CM_STR *cms_pp, int cnt, int swKey1, int swKey2);
void    comMakeSubSwapCntKey(CM_STR *cms_pp, int cnt, int swKey1, int swKey2, int pos);
void    comMakeSubDoubleKey(CM_STR *cms_pp, int cnt);
CM_STR* comMakeSubSpaceSearch(CM_STR *cms_pp, int cnt);
int     comMakeSubUseKeyCode(CM_STR *cms_pp, int cnt);

void comMakingNo0(CM_STR_CTRL *cmstr_pp);
void comMakingNo1(CM_STR_CTRL *cmstr_pp);
void comMakingNo2(CM_STR_CTRL *cmstr_pp);
void comMakingNo3(CM_STR_CTRL *cmstr_pp);
void comMakingNo5(CM_STR_CTRL *cmstr_pp);
void comMakingNo6(CM_STR_CTRL *cmstr_pp);
void comMakingNo7(CM_STR_CTRL *cmstr_pp);
void comMakingNo8(CM_STR_CTRL *cmstr_pp);
void comMakingNo9(CM_STR_CTRL *cmstr_pp);
void comMakingNo15(CM_STR_CTRL *cmstr_pp);
void comMakingNo16(CM_STR_CTRL *cmstr_pp);

int computerMaking(COMMAKE_STR *com_pp, int com_cnt, TAPDAT *moto_pp, int moto_cnt, TAPSET *tapset_pp, LEVEL_VS_ENUM clvl);

#endif /* COMMAKE_H */
