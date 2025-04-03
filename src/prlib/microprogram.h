#ifndef PRLIB_MICROPROGRAM_H
#define PRLIB_MICROPROGRAM_H

enum PrMICRO_PROGRAM_MODULE {
    ePrMicroProgram_Unk2 = 2,
};

int PrGetMicroProgramAddress(PrMICRO_PROGRAM_MODULE);

#endif