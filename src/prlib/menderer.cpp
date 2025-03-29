#include "common.h"

INCLUDE_ASM("prlib/menderer", InitializeNoodleStripRendering__FUiUiUiUi);

INCLUDE_ASM("prlib/menderer", GetRandom__Fv);

INCLUDE_ASM("prlib/menderer", StageIndexForColor__Fv);

INCLUDE_ASM("prlib/menderer", PushNoodleColor__FPUl);

INCLUDE_ASM("prlib/menderer", DrawNoodleStripChunk__FRCt8NaMATRIX3Zfi4i4);

INCLUDE_ASM("prlib/menderer", SetNoodleRotationMatrix__FRt8NaMATRIX3Zfi4i4f);

INCLUDE_ASM("prlib/menderer", PreDrawNoodleStrip__Fv);

INCLUDE_ASM("prlib/menderer", DrawNoodleStrip__Fff);

INCLUDE_ASM("prlib/menderer", StartNoodleRotation__Fv);

INCLUDE_ASM("prlib/menderer", UpdateNoodleRotation__Fv);

INCLUDE_ASM("prlib/menderer", PrUpdateMendererSpeed__Fv);

INCLUDE_ASM("prlib/menderer", PrDecelerateMenderer);

INCLUDE_ASM("prlib/menderer", PrRestartMenderer);

INCLUDE_ASM("prlib/menderer", DrawMenderer__Fv);

INCLUDE_ASM("prlib/menderer", PrSetMendererRatio);

INCLUDE_ASM("prlib/menderer", PrGetMendererRatio);

INCLUDE_ASM("prlib/menderer", PrSetMendererDirection);

INCLUDE_ASM("prlib/menderer", PrGetMendererDirection);

INCLUDE_ASM("prlib/menderer", PrSetMendererColorModulation);

INCLUDE_ASM("prlib/menderer", PrIsMendererColorModulation);

INCLUDE_ASM("prlib/menderer", PrInitializeMenderer);

INCLUDE_ASM("prlib/menderer", PrRenderMenderer);

/* nalib/navector.h */
INCLUDE_ASM("prlib/menderer", func_0014F3B8);

INCLUDE_ASM("prlib/menderer", func_0014F410);

/* prlib/menderer.cpp */
PR_EXTERN
void _GLOBAL_$I$prMendererRatio(void) {
}

/* nalib/navector.h */
INCLUDE_ASM("prlib/menderer", func_0014F4C8);

INCLUDE_ASM("prlib/menderer", func_0014F5D0);

INCLUDE_ASM("prlib/menderer", func_0014F6D8);

/* prlib/menderer.cpp */
INCLUDE_RODATA("prlib/menderer", D_003967E0);

INCLUDE_RODATA("prlib/menderer", D_003967F0);
