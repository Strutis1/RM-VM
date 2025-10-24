
#include "../../include/timer.h"
#include "../../include/common.h"
#include "../../include/registers.h"
#include "../../include/interrupts.h"
#include <stdlib.h>

void raiseTimerInterrupt(uint16_t code) {
    if (code >= TI_NONE && code <= TI_EXPIRED)
        realCPU.TI = code;
}

void initTimer(Timer *timer, uint16_t limit) {
    timer->counter = 0;
    timer->limit = limit;
    timer->active = 1;
    timer->interruptFlag = 0;
}

void tickTimer(Timer *timer) {
    if (!timer->active) return;

    timer->counter++;
    if (timer->counter >= timer->limit) {
        timer->interruptFlag = 1;
        timer->counter = 0;
    }
}

bool checkTimerInterrupt(Timer *timer) {
    if (timer->interruptFlag) {
        timer->interruptFlag = 0;
        return true;
    }
    return false;
}

void resetTimer(Timer *timer) {
    timer->counter = 0;
    timer->interruptFlag = 0;
}
