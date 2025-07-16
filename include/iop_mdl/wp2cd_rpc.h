#ifndef WP2CD_RPC_H
#define WP2CD_RPC_H

#include "common.h"

#include "iop_mdl/wp2cd/wp2cd.h"

int WP2Init(void);
int WP2Ctrl(int command, int data0);

#endif /* WP2CD_RPC_H */
