#pragma once
#include <stdint.h>
#include "rm.h"

// Returns 1 if RM should keep running, 0 if it should stop (in cases og HALT or critical fault)
int handleInterrupts(void);

void handleProgramInterrupt(void);
void handleSystemInterrupt(void);
void handleTimerInterrupt(void);
