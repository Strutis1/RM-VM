#include "rm_supervisor.h"


//some helpers
static inline void enterSupervisor(void) { realCPU.MODE = MODE_SUPERVISOR; }
static inline void leaveSupervisor(void) { realCPU.MODE = MODE_USER; } 


//handling of interrupts
void handleProgramInterrupt(void) {
    switch (realCPU.PI) {
        case PI_INVALID_OPCODE:
        case PI_INVALID_ADDRESS:
        case PI_INVALID_REGISTER:
        case PI_DIVZERO:
        case PI_OVERFLOW:
            realCPU.PI = PI_NONE;
            realCPU.SI = SI_HALT;
            return;
        case PI_NONE:
        default:
            break;
    }
    realCPU.PI = PI_NONE;
}

void handleSystemInterrupt(void) {
    switch (realCPU.SI) {
        case SI_READ:
            //do something with channel/device
            realCPU.SI = SI_NONE;
            break;
        case SI_WRITE:
            //do something with channel/device
            realCPU.SI = SI_NONE;
            break;
        case SI_SYS:
            // make use of syscall table
            realCPU.SI = SI_NONE;
            break;
        case SI_HALT:
            // we stop
            break;
        case SI_NONE:
        default:
            break;
    }
}

void handleTimerInterrupt(void) {
    if (realCPU.TI == TI_EXPIRED) {
        // for now, just clear and continue.
        realCPU.TI = TI_NONE;
    } else {
        realCPU.TI = TI_NONE;
    }
}

// Returns 0 to stop the RM (in cases of HALT or critical fault), 1 to continue
int handleInterrupts(void) {
    if (realCPU.PI == PI_NONE && realCPU.SI == SI_NONE && realCPU.TI == TI_NONE)
        return 1;

    enterSupervisor();

    if (realCPU.PI != PI_NONE)
        handleProgramInterrupt();

    if (realCPU.SI != SI_NONE)
        handleSystemInterrupt();

    if (realCPU.TI != TI_NONE)
        handleTimerInterrupt();

    if (realCPU.SI == SI_HALT) {
        //we stop
        return 0;
    }

    leaveSupervisor();

    return 1;
}