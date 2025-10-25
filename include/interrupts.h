#pragma once
#include "common.h"
#include <stdint.h>
#include <stdbool.h>
#include "registers.h"


// Interrupts
#define PI_NONE             0
#define PI_INVALID_OPCODE   1
#define PI_INVALID_ADDRESS  2
#define PI_INVALID_REGISTER 3
#define PI_DIVZERO          4
#define PI_PRIVILEGE_VIOLATION 5 //if user tries to access supervisor privilege
#define PI_OVERFLOW         6


#define SI_NONE             0
#define SI_READ             1
#define SI_WRITE            2
#define SI_HALT             3
#define SI_SYS              4

#define TI_NONE             0
#define TI_EXPIRED          1

void raiseProgramInterrupt(uint16_t code);
void raiseSystemInterrupt(uint16_t code);
void raiseTimerInterrupt(uint16_t code);
bool hasPendingInterrupt(void);
void clearAllInterrupts(void);