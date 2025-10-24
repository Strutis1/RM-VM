#pragma once
#include "common.h"
#include <stdint.h>
#include <stdbool.h>


// Interrupts
#define PI_NONE             0
#define PI_INVALID_OPCODE   1
#define PI_INVALID_ADDRESS  2
#define PI_OVERFLOW         3

#define SI_NONE             0
#define SI_READ             1
#define SI_WRITE            2
#define SI_HALT             3

#define TI_NONE             0
#define TI_EXPIRED          1

typedef struct {
    uint8_t PI;
    uint8_t SI;
    uint8_t TI;
} InterruptState;

void initInterrupts(InterruptState *intr);
bool hasInterrupt(const InterruptState *intr);
void raiseProgramInterrupt(InterruptState *intr, uint8_t code);
void raiseSupervisorInterrupt(InterruptState *intr, uint8_t code);
void raiseTimerInterrupt(InterruptState *intr);
void handleInterrupts(InterruptState *intr);