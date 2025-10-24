#pragma once

#include "../../include/common.h"
#include "../../include/registers.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/memory.h"
#include <stdlib.h>
#include <stdbool.h>


void raiseProgramInterrupt(uint16_t code);

void execCycle();