#pragma once

#include <stdint.h>
#include "../../include/registers.h"
#include "../kernel/scheduler.h"

typedef struct {
    uint16_t IC;
    uint16_t R[REG_COUNT];
    uint16_t SF;
    uint16_t PTR;
    uint16_t MODE;
} CPUContext;

void contextSwitch(Scheduler* sch, CPU* cpu);