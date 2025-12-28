#pragma once

#include <stdint.h>
#include "../../include/registers.h"
#include "../kernel/scheduler.h"

void saveContext(Process* proc, CPU* cpu);
void loadContext(Process* proc, CPU* cpu);
void contextSwitch(Scheduler* sch, CPU* cpu);
