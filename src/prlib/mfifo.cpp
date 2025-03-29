#include "common.h"

INCLUDE_ASM("prlib/mfifo", PrInitializeMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrCleanupMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrStartMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrStopMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrWaitMfifo__Fv);

INCLUDE_ASM("prlib/mfifo", PrSendMfifo__FPC10_sceDmaTag);

INCLUDE_ASM("prlib/mfifo", PrMfifoUnsentDataSize__Fv);
