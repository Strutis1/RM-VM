#pragma once

#include "../../include/common.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"
#include "../../include/io.h"
#include "../../include/registers.h"

Channel* initChannel();

char writeChannel(Channel* channel);
uint64_t* readChannel(Channel* channel);