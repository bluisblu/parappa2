#include "prlib/microprogram.h"

#include <libdma.h>

extern u_int vump_dmatag[]                  __attribute__((section(".vutext")));

extern u_int vump_top[]                     __attribute__((section(".vutext")));
extern u_int vump_normal[]                  __attribute__((section(".vutext")));
extern u_int vump_bothface[]                __attribute__((section(".vutext")));
extern u_int vump_contour[]                 __attribute__((section(".vutext")));
extern u_int vump_refmap[]                  __attribute__((section(".vutext")));
extern u_int vump_screen[]                  __attribute__((section(".vutext")));
extern u_int vump_antiline[]                __attribute__((section(".vutext")));
extern u_int vump_menderer_create_texture[] __attribute__((section(".vutext")));
extern u_int vump_menderer_draw_mesh[]      __attribute__((section(".vutext")));

/* .bss */
extern u_int MicroProgramEntryPoint[];

/* .sbss */
extern u_int MendererCreateTextureEntryPoint;
extern u_int MendererDrawMeshEntryPoint;

/* Divides address by 8 to be used on MSCAL */
#define VU_ADDR(x, base) (((int)x - (int)base) >> 3)

static void CalculateEntryPoint(void) {
    MicroProgramEntryPoint[eVumpNormal]   = VU_ADDR(vump_normal, vump_top);
    MicroProgramEntryPoint[eVumpBothface] = VU_ADDR(vump_bothface, vump_top);
    MicroProgramEntryPoint[eVumpContour]  = VU_ADDR(vump_contour, vump_top);
    MicroProgramEntryPoint[eVumpRefmap]   = VU_ADDR(vump_refmap, vump_top);
    MicroProgramEntryPoint[eVumpScreen]   = VU_ADDR(vump_screen, vump_top);
    MicroProgramEntryPoint[eVumpAntiline] = VU_ADDR(vump_antiline, vump_top);
    
    MendererCreateTextureEntryPoint = VU_ADDR(vump_menderer_create_texture, vump_top);
    MendererDrawMeshEntryPoint      = VU_ADDR(vump_menderer_draw_mesh, vump_top);
}

void PrLoadMicroPrograms() {
    sceDmaChan* chan = sceDmaGetChan(SCE_DMA_VIF1);

    /*
     * TTE is required to transfer
     * the first MPG VIFcodes inside
     * of the DMAtag's upper 64 bits:
     *
     * NOP,
     * MPG 0x0, 0x100 (2048 bytes)
     */
    chan->chcr.TTE = 1;
    
    sceDmaSend(chan, &vump_dmatag);
    CalculateEntryPoint();
}

u_int PrGetMicroProgramAddress(PrMICRO_PROGRAM_MODULE module) {
    return MicroProgramEntryPoint[module];
}

INCLUDE_ASM("prlib/microprogram", PrGetInputVertexParameterNum__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetInputVertexParameterSet__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetOutputVertexParameterNum__F22PrMICRO_PROGRAM_MODULE);

INCLUDE_ASM("prlib/microprogram", PrGetOutputVertexParameterSet__F22PrMICRO_PROGRAM_MODULE);

u_int PrGetMendererCreateTextureAddress() {
    return MendererCreateTextureEntryPoint;
}

u_int PrGetMendererDrawMeshAddress() {
    return MendererDrawMeshEntryPoint;
}
