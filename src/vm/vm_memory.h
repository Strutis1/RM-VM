#pragma once


#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel.h"
#include "../../include/device.h"


typedef struct {
    uint8_t memoryCells[VM_MEMORY_SIZE];
} VM_MEMORY;


VM_MEMORY* VMinitMemory();

// parses int64 and puts the corresponding hex thinggers into the instruction struct
char stuffInstructions(Instruction* ins, const uint64_t insRegion);
// loads the actual instructions into the memory
char loadProgram(Instruction* ins, VM_MEMORY* mem);