#ifndef CMNFILE_H
#define CMNFILE_H

#include "common.h"

typedef enum {
    CMN_NONE,  /* None */
    CMN_VRAM,  /* TIM2 textures */
    CMN_SND,   /* Sounds */
    CMN_ONMEM, /* Models, animations, etc. to store in the memory pool */
    CMN_MAX    /* N/A */
} CMN_FILE_TYPE_ENUM;

typedef struct { // 0x10
    /* 0x0 */ int fnum;
    /* 0x4 */ int ftype;
    /* 0x8 */ int f_size;
    /* 0xc */ int pad;
    /* 0x10 */ int adr[0];
} CMN_FILE_STR;

int cmnfTim2Trans(void);
void* cmnfGetFileAdrs(int num);
int cmnfGetFileSize(int num);

#endif /* CMNFILE_H */
