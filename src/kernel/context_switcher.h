#pragma once

#include <stdint.h>
#include "../kernel/scheduler.h"

void saveContext(Process* proc);
void loadContext(Process* proc);
void contextSwitch(Scheduler* sch);
