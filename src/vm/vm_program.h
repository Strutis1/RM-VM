#pragma once


#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/channel_device.h"
#include "../../include/registers.h"


char executeInstruction(Instruction* ins, const uint16_t insWord);