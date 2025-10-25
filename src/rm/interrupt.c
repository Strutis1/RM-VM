#include "../../include/interrupts.h"
#include "../../include/common.h"

void raiseProgramInterrupt(uint16_t code) {
    if (code <= PI_OVERFLOW)
        realCPU.PI = code;
}

void raiseSystemInterrupt(uint16_t code) {
    if (code <= SI_SYS)
        realCPU.SI = code;
}

void raiseTimerInterrupt(uint16_t code) {
    if (code <= TI_EXPIRED)
        realCPU.TI = code;
}

bool hasPendingInterrupt(void) {
    return (realCPU.PI != PI_NONE || realCPU.SI != SI_NONE || realCPU.TI != TI_NONE);
}

void clearAllInterrupts(void) {
    realCPU.PI = PI_NONE;
    realCPU.SI = SI_NONE;
    realCPU.TI = TI_NONE;
}