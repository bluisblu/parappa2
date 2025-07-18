#include "menu/menu_mdl.h"

#include "os/system.h"

#include "main/cdctrl.h"

#include "menu/menusub.h"

#include <prlib/prlib.h>

#include <math.h>
#include <string.h>

/* .data */
extern PRPROOT PRP_RootTbl[];

static u_int AMusicFitTime;

static void SetDrawEnv12(sceGsDrawEnv1 *pdenv);
static void MnMoveMode_InitRoot(int movNo);
static void _myVu0Length(float *v0, float *v1);
static void _MnParMovRoot_GetPos(PRPROOT *prt, float rate, float *pos);
static void MnMoveModelPosition(void *spm, int movNo, int ttim, int ttim0);

void MNScene_ExecAnime(MN_SCENE *pshdl);

void MNSceneMusicFitTimerClear(void) {
    AMusicFitTime = 0;
}

void MNSceneMusicFitTimerFrame(void) {
    AMusicFitTime++;
}

int MNSceneGetMusicFitTimer(void) {
    return AMusicFitTime;
}

void MNScene_Init(MN_SCENE *pshdl, MN_SCENETBL *tbl, int bFocus) {
    MN_HMDL   *mdl;
    MN_MDLTBL *mtbl;
    int       *ctbl;
    int        mn;
    int        i;

    mdl  = pshdl->mdl;
    ctbl = tbl->pctbl;
    mtbl = tbl->pmtbl;

    memset(pshdl, 0, sizeof(*pshdl));

    pshdl->scene = PrInitializeScene(&DBufDc.draw01, NULL, (bFocus) ? 0xd2 : -1);

    for (mn = 0; mn < PR_ARRAYSIZE(pshdl->mdl) && mtbl->fn_mdl != 0; mn++, mdl++, mtbl++) {
        float clear = 0.0f;

        mdl->spm = PrInitializeModel(GetIntAdrsCurrent(mtbl->fn_mdl), pshdl->scene);
        mdl->ablend_speed = clear;
        mdl->ablend_rate = clear;
        mdl->bABlend = 0;

        for (i = 0; i < PR_ARRAYSIZE(mtbl->fn_anmm) && mtbl->fn_anmm[i] != 0; i++) {
            mdl->spa_m[i] = PrInitializeAnimation(GetIntAdrsCurrent(mtbl->fn_anmm[i]));
        }
        for (i = 0; i < PR_ARRAYSIZE(mtbl->fn_anmp) && mtbl->fn_anmp[i] != 0; i++) {
            mdl->spa_p[i] = PrInitializeAnimation(GetIntAdrsCurrent(mtbl->fn_anmp[i]));
        }

        if (mdl->spa_m[0] != NULL) {
            PrLinkAnimation(mdl->spm, mdl->spa_m[0]);
            mdl->MSpaNo = 1;
        }
        if (mdl->spa_p[0] != NULL) {
            PrLinkPositionAnimation(mdl->spm, mdl->spa_p[0]);
            mdl->PSpaNo = 1;
        }

        PrShowModel(mdl->spm, NULL);
        mdl->dspSw = 1;
    }

    pshdl->nmdl = mn;

    for (mn = 0; mn < 14 && ctbl[mn] != NULL; mn++) {
        pshdl->spc[mn] = PrInitializeCamera(GetIntAdrsCurrent(ctbl[mn]));
    }

    pshdl->ncam = mn;

    if (pshdl->spc[0] != NULL) {
        PrSelectCamera(pshdl->spc[0], pshdl->scene);
        pshdl->CAniNo = 1;
    }

    PrPreprocessSceneModel(pshdl->scene);
}

void MNScene_End(MN_SCENE *pshdl) {
    MN_HMDL *mdl;
    int      mn;
    int      i;

    mdl = pshdl->mdl;
    for (mn = 0; mn < pshdl->nmdl; mn++, mdl++) {
        if (mdl->spm != NULL) {
            PrCleanupModel(mdl->spm);
        }

        for (i = 0; i < PR_ARRAYSIZE(mdl->spa_m); i++) {
            if (mdl->spa_m[i] != NULL) {
                PrCleanupAnimation(mdl->spa_m[i]);
            }
        }

        for (i = 0; i < PR_ARRAYSIZE(mdl->spa_p); i++) {
            if (mdl->spa_p[i] != NULL) {
                PrCleanupAnimation(mdl->spa_p[i]);
            }
        }
    }

    for (mn = 0; mn < pshdl->ncam; mn++) {
        if (pshdl->spc[mn] != NULL) {
            PrCleanupCamera(pshdl->spc[mn]);
        }
    }

    if (pshdl->scene != NULL) {
        PrCleanupScene(pshdl->scene);
    }

    memset(pshdl, 0, sizeof(*pshdl));
}

static void SetDrawEnv12(sceGsDrawEnv1 *pdenv) {
    DRAWENV_TAG12 denvTag;

    if (pdenv == NULL) {
        return;
    }

    ((u_long*)&denvTag.giftag)[0] = SCE_GIF_SET_TAG(8, 1, 0, SCE_GS_PRIM_POINT, 0, 1);
    ((u_long*)&denvTag.giftag)[1] = SCE_GIF_PACKED_AD;
    denvTag.denv1 = *pdenv;

    sceGsSyncPath(0, 0);
    FlushCache(WRITEBACK_DCACHE);

    sceGsPutDrawEnv(&denvTag.giftag);
    sceGsSyncPath(0, 0);

    denvTag.denv1.frame1addr    = SCE_GS_FRAME_2;
    denvTag.denv1.zbuf1addr     = SCE_GS_ZBUF_2;
    denvTag.denv1.xyoffset1addr = SCE_GS_XYOFFSET_2;
    denvTag.denv1.scissor1addr  = SCE_GS_SCISSOR_2;
    denvTag.denv1.test1addr     = SCE_GS_TEST_2;
    FlushCache(WRITEBACK_DCACHE);

    sceGsPutDrawEnv(&denvTag.giftag);
    sceGsSyncPath(0, 0);
}

void MNScene_Draw(MN_SCENE *pshdl) {
    if (pshdl->isDisp) {
        MNScene_ExecAnime(pshdl);

        if (pshdl->isDisp & 1) {
            PrSetSceneEnv(pshdl->scene, DrawGetDrawEnvP(DNUM_DRAW));
            PrRender(pshdl->scene);
            PrWaitRender();
        }

        if (pshdl->isDisp & 2) {
            TsMenu_CleanVram(DNUM_VRAM2);

            SetDrawEnv12(DrawGetDrawEnvP(DNUM_VRAM2));

            PrSetSceneEnv(pshdl->scene, DrawGetDrawEnvP(DNUM_VRAM2));
            PrRender(pshdl->scene);
            PrWaitRender();

            SetDrawEnv12(DrawGetDrawEnvP(DNUM_DRAW));
        }
    }
}

void MNScene_DispSw(MN_SCENE *pshdl, int sw) {
    pshdl->isDisp = sw;
}

void MNScene_SetAnimete(MN_SCENE *pshdl) {
    MN_HMDL *mdl;
    int      mn; 

    mdl = pshdl->mdl;
    for (mn = 0; mn < pshdl->nmdl; mn++, mdl++) {
        if (!mdl->dspSw) {
            continue;
        }

        PrAnimateModel(mdl->spm, pshdl->time[mdl->MAniNo]);
        PrAnimateModelPosition(mdl->spm, pshdl->time[mdl->PAniNo]);

        if (mdl->MvTblNo != 0) {
            MNANM_TBL *anm = pshdl->anime[mdl->PMovNo];
            if (anm != NULL) {
                MnMoveModelPosition(mdl->spm, mdl->MvTblNo - 1,
                                    pshdl->time[mdl->PMovNo], anm->etime);
            }
        }

        if (mdl->bABlend) {
            PrSetTransactionBlendRatio(mdl->spm, mdl->ablend_rate);
        }
    }

    PrAnimateSceneCamera(pshdl->scene, pshdl->time[pshdl->CAniNo]);
}

u_int MNScene_StartAnime(MN_SCENE *pshdl, int no, MNANM_TBL *anime) {
    MNANM_COBJ *acobj;
    int         i;
    u_int       anmBit;
    int         ano;
    MN_HMDL    *mdl;

    if (anime == NULL) {
        pshdl->anime[no] = NULL;
        return PR_BIT(31);
    }
    if (no < 0) {
        no = anime->aTimNo;
    }
    if (no >= 10) {
        no = 0;
    }

    pshdl->speed[no] = anime->aSpeed;
    pshdl->time[no] = anime->stime;
    pshdl->anime[no] = anime;
    
    acobj  = anime->anmCobj;
    anmBit = PR_BIT(no);

    for (i = 0; i < 6 && acobj->cflg != 0; i++, acobj++) {
        int anmBit = acobj->cflg;
        ano = anmBit & 0xff;
        switch (anmBit & 0xff00) {
        case PR_BIT(8) | PR_BIT(9) | PR_BIT(10) | PR_BIT(11):
            PrSelectCamera(pshdl->spc[acobj->no], pshdl->scene);
            pshdl->CAniNo = no;
            pshdl->CSpcNo = acobj->no + 1;
            break;
        case PR_BIT(8):
            mdl = &pshdl->mdl[ano];
            if (mdl->spm != NULL) {
                PrShowModel(mdl->spm, NULL);
                PrLinkAnimation(mdl->spm, mdl->spa_m[acobj->no]);

                mdl->MAniNo = no;
                mdl->MSpaNo = acobj->no + 1;
                mdl->dspSw  = 1;
            }
            break;
        case PR_BIT(13) | PR_BIT(8):
        case PR_BIT(12) | PR_BIT(8):
            mdl = &pshdl->mdl[ano];
            if (mdl->spm != NULL) {
                if (mdl->bABlend != 0) {
                    PrResetPosture(mdl->spm);
                }

                PrSavePosture(mdl->spm);
                PrShowModel(mdl->spm, NULL);
                PrLinkAnimation(mdl->spm, mdl->spa_m[acobj->no]);

                mdl->bABlend = 1;
                mdl->ablend_rate = 0.0f;

                if ((acobj->cflg & 0xff00) == (PR_BIT(12) | PR_BIT(8))) {
                    mdl->ablend_speed = 0.033333335f;
                } else {
                    mdl->ablend_speed = 0.1f;
                }

                mdl->MAniNo = no;
                mdl->MSpaNo = acobj->no + 1;
                mdl->dspSw  = 1;
            }
            break;
        case PR_BIT(9):
            mdl = &pshdl->mdl[ano];
            if (mdl->spm != NULL) {
                PrShowModel(mdl->spm, NULL);
                PrLinkPositionAnimation(mdl->spm, mdl->spa_p[acobj->no]);

                mdl->PAniNo = no;
                mdl->PSpaNo = acobj->no + 1;
                mdl->dspSw  = 1;
            }
            break;
        case PR_BIT(10):
            mdl = &pshdl->mdl[ano];
            if (mdl->spm != NULL) {
                MnMoveMode_InitRoot(acobj->no - 1);
                PrShowModel(mdl->spm, NULL);
                PrUnlinkPositionAnimation(mdl->spm);

                mdl->PMovNo  = no;
                mdl->MvTblNo = acobj->no;
                mdl->dspSw   = 1;
                mdl->PSpaNo  = 0;
            }
            break;
        case PR_BIT(11):
            mdl = &pshdl->mdl[ano];
            if (mdl->spm != NULL) {
                if (acobj->no != 0) {
                    mdl->dspSw = 1;
                    PrShowModel(mdl->spm, NULL);
                } else {
                    mdl->dspSw = 0;
                    PrHideModel(mdl->spm);
                }
            }
            break;
        }
    }

    MNScene_SetAnimete(pshdl);
    return anmBit;
}

void MNScene_ContinueAnime(MN_SCENE *pshdl, int no, MNANM_TBL *anime) {
    if (anime == NULL) {
        if (no < 11u) {
            pshdl->cntani[no] = NULL;
        }
    } else {
        if (no < 0) {
            no = anime->aTimNo;
        }
        if (no > 9) {
            no = 0;
        }

        pshdl->speed[no]  = anime->aSpeed;
        pshdl->cntani[no] = anime;
    }
}

void MNScene_StopAnime(MN_SCENE *pshdl,int no) {
    MNScene_StartAnime(pshdl, no, NULL);
    MNScene_ContinueAnime(pshdl, no, NULL);
}

void MNScene_ExecAnime(MN_SCENE *pshdl) {
    MN_HMDL   *mdl;
    int        mn;
    int        i;
    int        time;
    MNANM_TBL *panm;

    for (i = 0; i < 10; i++) {
        panm = pshdl->anime[i];
        if (panm == NULL) {
            continue;
        }

        if (panm->kind == 2) {
            time = AMusicFitTime * pshdl->speed[i];
            if (panm->etime > 0) {
                time = (time % panm->etime);
            }
        } else {
            time = pshdl->time[i] + pshdl->speed[i];

            if (panm->etime >= 0 && panm->etime < time) {
                if (panm->kind == 1) {
                    time = panm->stime;
                } else {
                    time = panm->etime;
                }
            }

            if (time == panm->etime) {
                if (pshdl->cntani[i] != NULL) {
                    MNScene_StartAnime(pshdl, i, pshdl->cntani[i]);
                    MNScene_ContinueAnime(pshdl, i, NULL);
                }
            }
        }

        pshdl->time[i] = time;
    }

    mdl = pshdl->mdl;
    for (mn = 0; mn < pshdl->nmdl; mn++, mdl++) {
        if (!mdl->bABlend) {
            continue;
        }

        if (mdl->ablend_rate >= 1.0f) {
            float clear = 0.0f;
            mdl->ablend_speed = clear;
            mdl->ablend_rate = clear;
            PrResetPosture(mdl->spm);
            mdl->bABlend = 0;
        } else {
            mdl->ablend_rate += mdl->ablend_speed;
            if (mdl->ablend_rate > 1.0f) {
                mdl->ablend_rate = 1.0f;
            }
        }
    }

    MNScene_SetAnimete(pshdl);
}

void MNScene_CopyState(MN_SCENE *pdhdl, MN_SCENE *pshdl) {
    MN_HMDL *mdl;
    MN_HMDL *smdl;
    int      i;

    memcpy(pdhdl->time, pshdl->time, sizeof(pdhdl->time));
    memcpy(pdhdl->anime, pshdl->anime, sizeof(pdhdl->anime));
    memcpy(pdhdl->cntani, pshdl->cntani, sizeof(pdhdl->cntani));
    memcpy(pdhdl->speed, pshdl->speed, sizeof(pdhdl->speed));

    pdhdl->CSpcNo = pshdl->CSpcNo;
    pdhdl->CAniNo = pshdl->CAniNo;

    if (pdhdl->CSpcNo != 0) {
        if (pdhdl->scene != NULL) {
            PrSelectCamera(pdhdl->spc[pdhdl->CSpcNo - 1], pdhdl->scene);
        }
    }

    mdl  = pdhdl->mdl;
    smdl = pshdl->mdl;

    pdhdl->nmdl = pshdl->nmdl;

    for (i = 0; i < pshdl->nmdl; i++, mdl++, smdl++) {
        f32 clear = 0.0f;

        mdl->dspSw = smdl->dspSw;
        mdl->ablend_speed = clear;
        mdl->ablend_rate  = clear;

        mdl->MAniNo  = smdl->MAniNo;
        mdl->PAniNo  = smdl->PAniNo;

        mdl->PMovNo  = smdl->PMovNo;
        mdl->MvTblNo = smdl->MvTblNo;

        mdl->MSpaNo  = smdl->MSpaNo;
        mdl->PSpaNo  = smdl->PSpaNo;

        mdl->bABlend = 0;

        if (mdl->spm != NULL) {
            if (mdl->dspSw != 0) {
                PrShowModel(mdl->spm, NULL);
            } else {
                PrHideModel(mdl->spm);
            }
        }

        if (mdl->MSpaNo != 0) {
            if (mdl->spm != NULL) {
                PrLinkAnimation(mdl->spm, mdl->spa_m[mdl->MSpaNo - 1]);
            }
        }

        if (mdl->PSpaNo != 0) {
            if (mdl->spm != NULL) {
                PrLinkPositionAnimation(mdl->spm, mdl->spa_p[mdl->PSpaNo - 1]);
            }
        }

        if (mdl->MvTblNo != 0) {
            PrUnlinkPositionAnimation(mdl->spm);
            MnMoveMode_InitRoot(mdl->MvTblNo - 1);
        }
    }
}

void MNScene_CopyStateMdl(MN_SCENE *pdhdl, MN_SCENE *pshdl) {
    MN_HMDL *mdl;
    MN_HMDL *smdl;
    int      i;

    memcpy(pdhdl->time, pshdl->time, sizeof(pdhdl->time));
    memcpy(pdhdl->anime, pshdl->anime, sizeof(pdhdl->anime));
    memcpy(pdhdl->cntani, pshdl->cntani, sizeof(pdhdl->cntani));
    memcpy(pdhdl->speed, pshdl->speed, sizeof(pdhdl->speed));

    mdl  = pdhdl->mdl;
    smdl = pshdl->mdl;
    for (i = 0; i < pshdl->nmdl; i++, mdl++, smdl++) {
        f32 clear = 0.0f;

        mdl->dspSw = smdl->dspSw;
        mdl->ablend_speed = clear;
        mdl->ablend_rate  = clear;

        mdl->MAniNo  = smdl->MAniNo;
        mdl->PAniNo  = smdl->PAniNo;

        mdl->PMovNo  = smdl->PMovNo;
        mdl->MvTblNo = smdl->MvTblNo;

        mdl->MSpaNo  = smdl->MSpaNo;
        mdl->PSpaNo  = smdl->PSpaNo;

        mdl->bABlend = 0;

        if (mdl->spm != NULL) {
            if (mdl->dspSw != 0) {
                PrShowModel(mdl->spm, NULL);
            } else {
                PrHideModel(mdl->spm);
            }
        }

        if (mdl->MSpaNo != 0) {
            if (mdl->spm != NULL) {
                PrLinkAnimation(mdl->spm, mdl->spa_m[mdl->MSpaNo - 1]);
            }
        }

        if (mdl->PSpaNo != 0) {
            if (mdl->spm != NULL) {
                PrLinkPositionAnimation(mdl->spm, mdl->spa_p[mdl->PSpaNo - 1]);
            }
        }

        if (mdl->MvTblNo != 0) {
            PrUnlinkPositionAnimation(mdl->spm);
            MnMoveMode_InitRoot(mdl->MvTblNo - 1);
        }
    }
}

void MNScene_SetAnimeSpeed(MN_SCENE *pshdl, int nAnime, int speed) {
    if (nAnime >= 10) {
        return;
    }

    pshdl->speed[nAnime] = speed;
}

void MNScene_SetAnimeEnd(MN_SCENE *pshdl) {
    int        i;
    MNANM_TBL *panm;

    for (i = 0; i < 10; i++) {
        panm = pshdl->anime[i];

        if (panm == NULL) {
            continue;
        }

        if (panm->kind - 1 > 1u) {
            pshdl->time[i] = panm->etime;
        }
    }

    MNScene_SetAnimete(pshdl);
}

void MNScene_SetAnimeBankEnd(MN_SCENE *pshdl, u_int bnk) {
    int        i;
    MNANM_TBL *panm;

    if ((int)bnk >= 0) {
        panm = pshdl->anime[bnk];

        if (panm != NULL) {
            if (panm->kind != 1 && panm->kind != 2) {
                pshdl->time[bnk] = panm->etime;
            }
        }
    } else {
        for (i = 0; i < 10; i++, bnk >>= 1) {
            if (!(bnk & 1)) {
                continue;
            }

            panm = pshdl->anime[i];
            if (panm != NULL) {
                if (panm->kind != 1 && panm->kind != 2) {
                    pshdl->time[i] = panm->etime;
                }
            }
        }
    }

    MNScene_SetAnimete(pshdl);
}

int MNScene_isAnime(MN_SCENE *pshdl, int ltim) {
    int        i;
    MNANM_TBL *panm;

    for (i = 0; i < 10; i++) {
        panm = pshdl->anime[i];
        if (panm != NULL) {
            if (panm->kind != 1 && panm->kind != 2) {
                if (pshdl->time[i] < (panm->etime - ltim)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int MNScene_isAnimeBank(MN_SCENE *pshdl, int ltim, u_int bnk) {
    int        i;
    MNANM_TBL *panm;

    if ((int)bnk >= 0) {
        panm = pshdl->anime[bnk];
        if (panm != NULL) {
            if (panm->kind != 1 && panm->kind != 2) {
                if (pshdl->time[bnk] < (panm->etime - ltim)) {
                    return 1;
                }
            }
        }
    } else {
        for (i = 0; i < 10; i++, bnk >>= 1) {
            if (!(bnk & 1)) {
                continue;
            }
            panm = pshdl->anime[i];
            if (panm != NULL) {
                if (panm->kind != 1 && panm->kind != 2) {
                    if (pshdl->time[i] < (panm->etime - ltim)) {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

int MNScene_isSeniAnime(MN_SCENE *pshdl) {
    int      mn;
    MN_HMDL *mdl = pshdl->mdl;

    for (mn = 0; mn < pshdl->nmdl; mn++, mdl++) {
        if (mdl->bABlend) {
            return 1;
        }
    }

    return 0;
}

int MNScene_ModelDispSw(MN_SCENE *pshdl, int nmdl, int bsw) {
    int      ret;
    MN_HMDL *mdl;

    if (pshdl->nmdl <= nmdl) {
        return 0;
    }
    
    mdl = &pshdl->mdl[nmdl];
    if (mdl->spm == NULL) {
        return 0;
    }

    ret = mdl->dspSw;
    mdl->dspSw = bsw;

    if (bsw != 0) {
        PrShowModel(mdl->spm, NULL);
        return ret;
    } else {
        PrHideModel(mdl->spm);
        return ret;
    }
}

/* 
    Calculates the length of a vector
       - v0 -> Output vector
       - v1 -> Input vector
*/
static void _myVu0Length(float *v0, float *v1) {
    asm volatile(
        "lqc2     vf04, 0x0(%1)      \n\t" /* Load v1(vf04) */
                                           /* vf04 = v1     */

        "vmul.xyz vf05, vf04, vf04   \n\t" /* vf05.xyz = (vf04.xyz)^2 */
        "vaddy.x  vf05, vf05, vf05y  \n\t" /* vf05.x  += vf05.y       */
        "vaddz.x  vf05, vf05, vf05z  \n\t" /* vf05.x  += vf05.z       */

        "vsqrt    Q, vf05x           \n\t" /* sqrt(vf05.x)  */
        "vwaitq                      \n\t" /* wait for sqrt */

        "vaddq.x  vf05, vf00, Q      \n\t" /* vf05 = Q   */
        "qmfc2    $8,   vf05         \n\t" /* $t0 = vf05 */

        "sw       $8,   0x0(%0)      \n\t" /* save v0  */
                                           /* v0 = $t0 */
    : : "r"(v0), "r"(v1));
}

static void MnMoveMode_InitRoot(int movNo) {
    int      i;

    PRPROOT *prt;
    PRPOS   *ppos;
    float    flen;

    prt  = &PRP_RootTbl[movNo & ~128];
    ppos = prt->ppos;
    flen = 0.0f;

    for (i = 1; i < prt->npos; i++) {
        float flen0;
        sceVu0FVECTOR v0;
        sceVu0FVECTOR v1;

        v0[0] = ppos[i - 1].x;
        v0[1] = ppos[i - 1].y;
        v0[2] = ppos[i - 1].z;
        v0[3] = 0.0f;

        v1[0] = ppos[i].x;
        v1[1] = ppos[i].y;
        v1[2] = ppos[i].z;
        v1[3] = 0.0f;

        sceVu0SubVector(v0, v1, v0);
        _myVu0Length(&flen0, v0);
        flen += flen0;

        ppos[i].dist = flen;
        ppos[i].rly = atan2f(v0[0], v0[2]);
    }

    ppos->dist = 0.0f;

    if (prt->npos >= 2) {
        ppos->rly = ppos[1].rly;
    } else {
        ppos->rly = 0.0f;
    }

    if (flen == 0.0f) {
        flen = 1.0f;
    }

    flen = (1.0f / flen);
    for (i = 1; i < prt->npos; i++) {
        ppos[i].dist *= flen;
    }
}

static void _MnParMovRoot_GetPos(PRPROOT *prt, float rate, float *pos) {
    PRPOS *ppos;
    PRPOS *psrc;
    PRPOS *pdst;
    int    i;

    ppos = prt->ppos;
    i    = 0;

    if (rate < 1.0f) {
        for (i = 1; i < prt->npos; i++) {
            if (rate < ppos[i].dist) break;
        }
    }

    if (rate >= 1.0f || i > prt->npos) {
        psrc = &ppos[prt->npos] - 1;
        pos[0] = psrc->x;
        pos[1] = psrc->y;
        pos[2] = psrc->z;
        pos[3] = psrc->rly;
        return;
    }

    pdst = &ppos[i];
    psrc = &ppos[i - 1];

    if (pdst->dist == psrc->dist) {
        rate = 0.0f;
    } else {
        rate = (rate - psrc->dist) / (pdst->dist - psrc->dist);
    }

    pos[0] = ((pdst->x - psrc->x) * rate) + psrc->x;
    pos[1] = ((pdst->y - psrc->y) * rate) + psrc->y;
    pos[2] = ((pdst->z - psrc->z) * rate) + psrc->z;
    pos[3] = pdst->rly;
}

static void MnMoveModelPosition(void *spm, int movNo, int ttim, int ttim0) {
    float         rate;
    float         WRATE;
    float         fry;
    sceVu0FVECTOR cpos;
    sceVu0FMATRIX mt;

    WRATE = 0.17f;

    if (ttim >= ttim0) {
        fry = 1.0f;
    } else {
        fry = (float)ttim / (float)ttim0;
    }  

    if (spm == NULL) {
        return;
    }

    rate = sinf(fry * 0.5f * (float)M_PI);

    if (movNo & 0x80) {
        rate = 1.0f - rate;
    }

    _MnParMovRoot_GetPos(&PRP_RootTbl[movNo & ~0x80], rate, cpos);

    if (rate == 0.0f || rate == 1.0f) {
        fry = 0.0f;
    } else {
        fry = cpos[3];

        if (movNo & 0x80) {
            fry += (float)M_PI;
        }
        if (fry > (float)M_PI) {
            fry -= ((float)M_PI * 2.0f);
        }
    }

    sceVu0UnitMatrix(mt);
    sceVu0RotMatrixY(mt, mt, fry);

    mt[3][0] = cpos[0] * WRATE;
    mt[3][1] = cpos[1] * WRATE;
    mt[3][2] = cpos[2] * WRATE;
    mt[3][3] = WRATE;

    PrShowModel(spm, &mt);
}
