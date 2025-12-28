//Wasnt sure where to put this so put it here for now

#include "context_switcher.h"
#include "scheduler.h"
#include <string.h>
#include <stdio.h>

void saveContext(Process* proc, CPU* cpu)
{
    if (!proc || !cpu) return;

    proc->context.IC   = cpu->IC;
    memcpy(proc->context.R, cpu->R, sizeof(cpu->R));
    proc->context.SF   = cpu->SF;
    proc->context.PTR  = cpu->PTR;
    proc->context.MODE = cpu->MODE;

    proc->state = PROC_READY;

    _log("Saved context\n");
}


void loadContext(Process* proc, CPU* cpu)
{
    if (!proc || !cpu) return;

    cpu->IC   = proc->context.IC;
    memcpy(cpu->R, proc->context.R, sizeof(cpu->R));
    cpu->SF   = proc->context.SF;
    cpu->PTR  = proc->context.PTR;
    cpu->MODE = proc->context.MODE;

    proc->state = PROC_RUNNING;

    _log("Loaded context\n");
}


void contextSwitch(Scheduler* sch, CPU* cpu) {
    if (!sch || !cpu) return;

    Process* prev = sch->current;
    Process* next = NULL;

    for (int i = sch->prio_max; i >= sch->prio_min; --i) {
        if (sch->schedule[i] &&
            sch->schedule[i]->state == PROC_READY) {
            next = sch->schedule[i];
            break;
        }
    }

    if (!next || next == prev) return;

    // Save
    if (prev) {
        prev->context.IC = cpu->IC;
        prev->context.SF = cpu->SF;
        prev->context.MODE = cpu->MODE;
        prev->context.PTR = cpu->PTR;
        prev->context.PI = cpu->PI;
        prev->context.SI = cpu->SI;
        prev->context.TI = cpu->TI;

        for (int i = 0; i < REG_COUNT; ++i)
            prev->context.R[i] = cpu->R[i];

        prev->state = PROC_READY;
    }

    // Load
    cpu->IC = next->context.IC;
    cpu->SF = next->context.SF;
    cpu->MODE = next->context.MODE;
    cpu->PTR = next->context.PTR;
    cpu->PI = next->context.PI;
    cpu->SI = next->context.SI;
    cpu->TI = next->context.TI;

    for (int i = 0; i < REG_COUNT; ++i)
        cpu->R[i] = next->context.R[i];

    next->state = PROC_RUNNING;
    sch->current = next;
}