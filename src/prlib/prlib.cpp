#include "prlib/prpriv.h"

#include "prlib/animation.h"
#include "prlib/camera.h"
#include "prlib/database.h"
#include "prlib/model.h"
#include "prlib/random.h"
#include "prlib/renderstuff.h"
#include "prlib/scene.h"

#include <nalib/navector.h>

#include <eetypes.h>
#include <eestruct.h>
#include <libgraph.h>

/* sdata */
static float prFrameRate = 1.0f;
static float prInverseFrameRate = 1.0f;

/* sbss */
extern PrDebugParam debugParam[2];

static void InitializeDebugParam();

PR_EXTERN
void PrSetFrameRate(float frameRate) {
    prFrameRate = frameRate;
    prInverseFrameRate = 1.0f / frameRate;
}

PR_EXTERN
float PrGetFrameRate() {
    return prFrameRate;
}

PR_EXTERN
void PrInitializeModule(sceGsZbuf zbuf) {
    InitializeDebugParam();
    PrInitializeRandomPool();

    prObjectDatabase.Initialize();
    prRenderStuff.Initialize(zbuf);
}

PR_EXTERN
void PrCleanupModule() {
    prRenderStuff.Cleanup();

    PrCleanupModel(NULL);
    PrCleanupAnimation(NULL);
    PrCleanupCamera(NULL);
    PrCleanupScene(NULL);

    prObjectDatabase.Cleanup();
}

PR_EXTERN
PrSceneObject* PrInitializeScene(sceGsDrawEnv1 *drawEnv, const char *name, u_int fbp) {
    return prObjectDatabase.CreateScene(drawEnv, name, fbp);
}

INCLUDE_ASM("prlib/prlib", PrInitializeSceneDBuff);

PR_EXTERN
void PrCleanupScene(PrSceneObject *scene) {
    if (scene == NULL) {
        scene = prObjectDatabase.unk0;
        while (scene != NULL) {
            prObjectDatabase.DeleteScene(scene);
            scene = prObjectDatabase.unk0;
        }
    } else {
        prObjectDatabase.DeleteScene(scene);
    }
}

PR_EXTERN
void PrSetSceneFrame() {
    /* Empty */
}

PR_EXTERN
void PrSetSceneEnv(PrSceneObject *scene, sceGsDrawEnv1 *drawEnv) {
    scene->unk50 = drawEnv->frame1;
    scene->unk58 = drawEnv->xyoffset1;
}

PR_EXTERN
void PrPreprocessSceneModel(PrSceneObject *scene) {
    scene->PreprocessModel();
}

INCLUDE_ASM("prlib/prlib", PrInitializeModel);

INCLUDE_ASM("prlib/prlib", PrInitializeAnimation);

INCLUDE_ASM("prlib/prlib", PrInitializeCamera);

INCLUDE_ASM("prlib/prlib", PrCleanupModel);

INCLUDE_ASM("prlib/prlib", PrCleanupAnimation);

INCLUDE_ASM("prlib/prlib", PrCleanupCamera);

INCLUDE_ASM("prlib/prlib", PrCleanupAllSceneModel);

PR_EXTERN
float PrGetAnimationStartFrame(SpaFileHeader *animation) {
    return 0.0f;
}

PR_EXTERN
float PrGetAnimationEndFrame(SpaFileHeader *animation) {
    return animation->unk14 * prFrameRate;
}

PR_EXTERN
float PrGetCameraStartFrame(SpcFileHeader *camera) {
    return 0.0f;
}

INCLUDE_ASM("prlib/prlib", PrGetCameraEndFrame);

INCLUDE_ASM("prlib/prlib", PrSetModelUserData);

INCLUDE_ASM("prlib/prlib", PrSetAnimationUserData);

INCLUDE_ASM("prlib/prlib", PrSetCameraUserData);

INCLUDE_ASM("prlib/prlib", PrGetModelUserData);

INCLUDE_ASM("prlib/prlib", PrGetAnimationUserData);

INCLUDE_ASM("prlib/prlib", PrGetCameraUserData);

INCLUDE_ASM("prlib/prlib", PrLinkAnimation);

INCLUDE_ASM("prlib/prlib", PrUnlinkAnimation);

INCLUDE_ASM("prlib/prlib", PrGetLinkedAnimation);

INCLUDE_ASM("prlib/prlib", PrLinkPositionAnimation);

INCLUDE_ASM("prlib/prlib", PrUnlinkPositionAnimation);

INCLUDE_ASM("prlib/prlib", PrGetLinkedPositionAnimation);

INCLUDE_ASM("prlib/prlib", PrSelectCamera);

INCLUDE_ASM("prlib/prlib", PrGetSelectedCamera);

INCLUDE_ASM("prlib/prlib", PrGetCurrentCamera);

INCLUDE_ASM("prlib/prlib", PrSetDefaultCamera);

INCLUDE_ASM("prlib/prlib", PrSetAppropriateDefaultCamera);

INCLUDE_ASM("prlib/prlib", PrShowModel);

INCLUDE_ASM("prlib/prlib", PrGetModelMatrix);

INCLUDE_ASM("prlib/prlib", PrHideModel);

PR_EXTERN
NaVECTOR<float, 4>* PrGetModelPrimitivePosition(PrModelObject *model) {
    /*
     * FIXME(poly): These `tmp_X` symbols aren't real
     * and are product of the static NaVECTOR instances.
     *
     * The solution would be to make these static
     * (when the TU's .bss matches, of course), and
     * the compiler should generate these temp. symbols.
     *
     * The symbols themselves are meant to call the
     * constructor once, though in this case there's
     * no actual call because it is inlined (empty constructor).
     */
    extern NaVECTOR<float, 4> vector_0;
    static int tmp_0 = 0;
    
    if (tmp_0 == 0) {
        tmp_0 = 1;
    }
    
    model->GetPrimitivePosition(&vector_0);
    return &vector_0;
}

PR_EXTERN
NaVECTOR<float, 4>* PrGetModelScreenPosition(PrModelObject *model) {
    /*
     * FIXME(poly): These `tmp_X` symbols aren't real
     * and are product of the static NaVECTOR instances.
     *
     * The solution would be to make these static
     * (when the TU's .bss matches, of course), and
     * the compiler should generate these temp. symbols.
     *
     * The symbols themselves are meant to call the
     * constructor once, though in this case there's
     * no actual call because it is inlined (empty constructor).
     */
    extern NaVECTOR<float, 4> vector_1;
    static int tmp_1 = 0;
    
    if (tmp_1 == 0) {
        tmp_1 = 1;
    }

    model->GetScreenPosition(&vector_1);
    return &vector_1;
}

int prCurrentStage = 0;

INCLUDE_ASM("prlib/prlib", PrAnimateModel);

INCLUDE_ASM("prlib/prlib", PrAnimateModelPosition);

INCLUDE_ASM("prlib/prlib", PrAnimateSceneCamera);

INCLUDE_ASM("prlib/prlib", PrRender);

INCLUDE_ASM("prlib/prlib", PrWaitRender);

PR_EXTERN
void PrSetStage(int stage) {
    prCurrentStage = stage;
}

INCLUDE_ASM("prlib/prlib", PrSetDepthOfField);

INCLUDE_ASM("prlib/prlib", PrSetDepthOfFieldLevel);

INCLUDE_ASM("prlib/prlib", PrGetFocalLength);

INCLUDE_ASM("prlib/prlib", PrGetDefocusLength);

INCLUDE_ASM("prlib/prlib", PrGetDepthOfFieldLevel);

INCLUDE_ASM("prlib/prlib", PrSaveContour);

INCLUDE_ASM("prlib/prlib", PrResetContour);

INCLUDE_ASM("prlib/prlib", PrSavePosture);

INCLUDE_ASM("prlib/prlib", PrResetPosture);

INCLUDE_ASM("prlib/prlib", PrSetContourBlurAlpha);

INCLUDE_ASM("prlib/prlib", PrSetTransactionBlendRatio);

INCLUDE_ASM("prlib/prlib", PrGetContourBlurAlpha);

INCLUDE_ASM("prlib/prlib", PrGetContourBlurAlpha2);

INCLUDE_ASM("prlib/prlib", PrGetTransactionBlendRatio);

INCLUDE_ASM("prlib/prlib", PrSetModelDisturbance);

INCLUDE_ASM("prlib/prlib", PrGetModelDisturbance);

INCLUDE_ASM("prlib/prlib", PrGetVertexNum);

INCLUDE_ASM("prlib/prlib", PrGetModelName);

INCLUDE_ASM("prlib/prlib", PrGetAnimationName);

INCLUDE_ASM("prlib/prlib", PrGetCameraName);

INCLUDE_ASM("prlib/prlib", PrGetSceneName);

INCLUDE_ASM("prlib/prlib", PrGetRenderingStatistics);

INCLUDE_ASM("prlib/prlib", PrSetModelVisibility);

INCLUDE_ASM("prlib/prlib", PrGetModelImage);

INCLUDE_ASM("prlib/prlib", PrGetAnimationImage);

INCLUDE_ASM("prlib/prlib", PrGetCameraImage);

PR_EXTERN
void PrSetDebugParam(int param, int value) {
    debugParam[param].d = value;
}

PR_EXTERN
void PrSetDebugParamFloat(int param, float value) {
    debugParam[param].d = *(int*)&value;
}

PR_EXTERN
int PrGetDebugParam(int param) {
    return debugParam[param].d;
}

PR_EXTERN
float PrGetDebugParamFloat(int param) {
    return debugParam[param].f;
}

static void InitializeDebugParam() {
    PrSetDebugParamFloat(PR_FLOAT_PARAM_DISTURBANCE, 1.0f);
}
