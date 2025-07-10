#ifndef PRLIB_PRPRIV_H
#define PRLIB_PRPRIV_H

#include "common.h"

#include "prlib/animation.h"
#include "prlib/camera.h"
#include "prlib/model.h"
#include "prlib/scene.h"

typedef struct { // 0x40
    /* 0x00 */ float position[4];
    /* 0x10 */ float interest[4];
    /* 0x20 */ float up[4];
    /* 0x30 */ float aspect;
    /* 0x34 */ float field_of_view;
    /* 0x38 */ float near_clip;
    /* 0x3c */ float far_clip;
} PrPERSPECTIVE_CAMERA;

typedef enum {
    PR_FLOAT_PARAM_DISTURBANCE = 0,
    PR_DEBUG_PARAM_NUM = 1
} PrDEBUG_PARAM;

typedef union {
    int   d;
    float f;
} PrDebugParam;

PR_EXTERN {

void PrCleanupModel(PrModelObject *model);
void PrCleanupAnimation(SpaFileHeader *model);
void PrCleanupCamera(SpcFileHeader *camera);
void PrCleanupScene(PrSceneObject *scene);

}

#endif /* PRLIB_PRPRIV_H */
