#ifndef TAPCTRL_RPC_H
#define TAPCTRL_RPC_H

#include "common.h"

int TapCtInit(void);
int TapCt(int command, int data1, int data2);

#endif /* TAPCTRL_RPC_H */
