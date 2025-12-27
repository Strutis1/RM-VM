#pragma once

#include "../../include/common.h"
#include "../../include/registers.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/memory.h"
#include "../../include/channel_device.h"
#include "../../include/io.h"
#include "../../include/disk.h"
#include "../../include/timer.h"
#include <stdlib.h>
#include <stdbool.h>


//void raiseProgramInterrupt(uint16_t code);

void initCPU(CPU *cpu);

void execCycle();