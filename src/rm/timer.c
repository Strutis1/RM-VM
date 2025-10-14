#include "rm.h"





void raiseTimerInterrupt(uint16_t code) {
    if (code >= TI_NONE && code <= TI_EXPIRED)
        realCPU.TI = code;
}