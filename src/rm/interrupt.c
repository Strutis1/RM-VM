#include "../../include/interrupts.h"
#include "../../include/common.h"

void initInterrupts(InterruptState *intr) {
    intr->PI = PI_NONE;
    intr->SI = SI_NONE;
    intr->TI = TI_NONE;
}

bool hasInterrupt(const InterruptState *intr) {
    return (intr->PI != PI_NONE || intr->SI != SI_NONE || intr->TI != TI_NONE);
}
