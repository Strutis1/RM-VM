#include "rm.h"



void raiseSystemInterrupt(uint16_t code) {
    if (code >= SI_NONE && code <= SI_HALT)
        realCPU.SI = code;
}
