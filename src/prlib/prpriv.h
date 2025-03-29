#ifndef PRLIB_PRPRIV_H
#define PRLIB_PRPRIV_H

#include "common.h"

#include "prlib/animation.h"
#include "prlib/camera.h"
#include "prlib/model.h"
#include "prlib/scene.h"

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

#endif