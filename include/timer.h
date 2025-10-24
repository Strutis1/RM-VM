#pragma once
#include "common.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    uint16_t counter;
    uint16_t limit;
    uint8_t active;        
    uint8_t interruptFlag;
} Timer;

void initTimer(Timer *timer, uint16_t limit);
void tickTimer(Timer *timer); //was bool but conflicting
void raiseTimerInterrupt(uint16_t code);
bool checkTimerInterrupt(Timer *timer);
void resetTimer(Timer *timer);
