


#include <stdio.h>
#include "rm/rm.h"
#include "utils/utils.h"
#include "interrupts.h"
#include "kernel/scheduler.h"
#include "kernel/kernel.h"
#include "kernel/loader.h"
#include "kernel/resourceManager.h"
#include "kernel/ioHandler.h"
#include "kernel/interruptHandler.h"
#include "vm/vm.h"
#include "memory.h"
#include <stdlib.h>

// Startup helper
static void onStartupPrompts(void) {
    generateConfig();
}

// // CPU + instruction execution test
// static void testCPUProgram(void) {
//     _log("\n[TEST] CPU instruction execution\n");

//     /*
//         Program:
//             LOAD R0, #1
//             LOAD R1, #2
//             ADD  R0, R1
//             HALT
//     */

//     physicalMemory.cells[0] = 0x1001;
//     physicalMemory.cells[1] = 0x2002;
//     physicalMemory.cells[2] = 0x3001;
//     physicalMemory.cells[3] = 0x0000;

//     realCPU.IC = 0;
//     realCPU.MODE = MODE_SUPERVISOR;

//     execCycle();

//     char buf[80];
//     snprintf(buf, sizeof(buf), "[RESULT] R0 = %u (expected 3)\n", realCPU.R[0]);
//     _log(buf);
//     snprintf(buf, sizeof(buf), "[RESULT] R1 = %u (expected 2)\n", realCPU.R[1]);
//     _log(buf);
// }

// // interrupt test
// static void testProgramInterrupt(void) {
//     _log("\n[TEST] Program interrupt (DIVZERO)\n");

//     physicalMemory.cells[0] = 0x1001; // LOAD R0, #1
//     physicalMemory.cells[1] = 0x2000; // LOAD R1, #0
//     physicalMemory.cells[2] = 0x6001; // DIV R0, R1
//     physicalMemory.cells[3] = 0x0000; // HALT

//     realCPU.IC = 0;
//     realCPU.MODE = MODE_SUPERVISOR;
//     realCPU.PI = PI_NONE;

//     execCycle();

//     char buf[80];
//     snprintf(buf, sizeof(buf), "[RESULT] PI = %u (expected %u)\n", realCPU.PI, PI_DIVZERO);
//     _log(buf);
// }

// // MEM proc test
// static void testMemoryProtection(void) {
//     _log("\n[TEST] Memory protection\n");

//     realCPU.MODE = MODE_USER;
//     realCPU.PI = PI_NONE;

//     // Attempt supervisor memory access
//     read(&physicalMemory, SUPERVISOR_MEMORY_START);

//     char buf[80];
//     snprintf(buf, sizeof(buf), "[RESULT] PI = %u (expected %u)\n",
//              realCPU.PI, PI_INVALID_ADDRESS);
//     _log(buf);
// }

// // Timer interrupt test
// static void testTimerInterrupt(void) {
//     _log("\n[TEST] Timer interrupt\n");

//     realCPU.TI = TI_NONE;

//     for (int i = 0; i < TICK_LIMIT + 1; ++i) {
//         raiseTimerInterrupt(TI_EXPIRED);
//     }

//     char buf[80];
//     snprintf(buf, sizeof(buf), "[RESULT] TI = %u (expected %u)\n",
//              realCPU.TI, TI_EXPIRED);
//     _log(buf);
// }

//Job Governor entry: load program by FileID into VM, run VM, finish.
static int jobGovernorEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)kernel;
    if (!proc || !proc->vm || !scheduler) return 0;

    // Request load once
    if (proc->loaded == 0) {
        Process* loaderProc = initProcess(0, true, "Loader", LOADER, NULL, (ProcessEntry)loaderEntry, fileId);
        loaderProc->parentPid = proc->pid;
        addProc(scheduler, loaderProc);
        proc->loaded = 2; // loading in progress
        proc->state = BLOCKED;
        scheduler->current = NULL;
        return -1; // stay blocked
    }

    // Still waiting for loader
    if (proc->loaded == 2) {
        proc->state = BLOCKED;
        scheduler->current = NULL;
        return -1;
    }

    //Loaded, run VM
    if (proc->loaded == 1) {
        int rc = runOperations(proc->vm);
        if (rc == 1) { // HALT
            if (proc->vm && proc->vm->vm_cpu) {
                char buf[128];
                snprintf(buf, sizeof(buf), "[JG] VM pid %u halted. R0=%u R1=%u R2=%u\n",
                         proc->pid,
                         proc->vm->vm_cpu->R[0],
                         proc->vm->vm_cpu->R[1],
                         proc->vm->vm_cpu->R[2]);
                _log(buf);
            }
            destroyVM(proc->vm);
            proc->vm = NULL;
            proc->loaded = 3;
            return 0; //finished
        } else if (rc == 2) { //trap (I/O), keep VM and stay blocked
            if (proc->vm) {
                char buf[96];
                snprintf(buf, sizeof(buf), "[JG] VM pid %u trapped (code=%u) r0=%u r1=%u r2=%u\n",
                         proc->pid, proc->vm->trap_code, proc->vm->trap_r0, proc->vm->trap_r1, proc->vm->trap_r2);
                _log(buf);
                // enqueue I/O so the IO handler can service it
                enqueueIORequest(proc,
                                 proc->vm->trap_code,
                                 proc->vm->trap_r0,
                                 proc->vm->trap_r1,
                                 proc->vm->trap_r2);
            }
            proc->state = BLOCKED;
            scheduler->current = NULL;
            return -1;
        } else if (rc < 0) { //error: stop this task
            destroyVM(proc->vm);
            proc->vm = NULL;
            proc->loaded = 3;
            return 0;
        }
        //otherwise keep running
        return 1;
    }

    return 0; //already done
}

//Idle entry: do nothing, stay ready.
static int idleEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)proc; (void)scheduler; (void)kernel; (void)fileId;
    return 1; // remain ready
}

static int startStopEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)fileId;
    if (!scheduler) return 0;

    bool hasWork = false;
    for (int p = SCHEDULER_MIN_PRIORITY; p < SCHEDULER_MAX_PRIORITY; ++p) {
        Process* pr = scheduler->schedule[p];
        if (!pr) continue;
        if (pr->procType == IDLE || pr->procType == START_STOP) continue;
        if (pr->state == READY || pr->state == BLOCKED) { hasWork = true; break; }
    }
    if (hasWork) return 1;

    //No work left
    for (int p = SCHEDULER_MIN_PRIORITY; p < SCHEDULER_MAX_PRIORITY; ++p) {
        Process* pr = scheduler->schedule[p];
        if (!pr || pr->procType == START_STOP) continue;
        if (pr->vm) {
            destroyVM(pr->vm);
            pr->vm = NULL;
        }
        scheduler->schedule[p] = NULL;
    }
    if (kernel && kernel->timer) {
        kernel->timer->active = 0;
    }
    _log("[START-STOP] System shutdown complete.\n");
    return 0;
}

int main(void) {

    onStartupPrompts();


    _log("[MAIN] Initializing Real Machine...\n");
    initRealMachine();
    initCPU(&realCPU);
    Timer systemTimer = {0};
    initTimer(&systemTimer, TICK_LIMIT);

    Kernel kernel = {0};

    Scheduler* scheduler = initScheduler();
    if (!scheduler) {
        _log("[MAIN] Failed to initialize scheduler.\n");
        return 1;
    }
    // Enable scheduler verbose by default (lightweight and useful)
    schedulerSetVerbose(true);
    _log("[MAIN] Scheduler verbose logging enabled.\n");

    initKernelState(&kernel, scheduler, &physicalMemory, &realCPU, &hardDisk, NULL, &systemTimer);

    _log("[MAIN] Creating processes...\n");

    initFileTable();

    // first program with header (wordCount=4, entry=0, loadAddr=0)
    uint8_t vmSector1[DISK_SECTOR_SIZE] = {0};
    vmSector1[0] = 0x04; vmSector1[1] = 0x00; // wordCount
    vmSector1[2] = 0x00; vmSector1[3] = 0x00; // entryOffset
    vmSector1[4] = 0x00; vmSector1[5] = 0x00; // loadAddress
    vmSector1[6] = 0x01; vmSector1[7] = 0x10; // 0x1001 (LOAD R0,#1) little endian
    vmSector1[8] = 0x02; vmSector1[9] = 0x12; // 0x1202 (LOAD R1,#2)
    vmSector1[10] = 0x40; vmSector1[11] = 0x30; // 0x3040 (ADD R0,R1)
    vmSector1[12] = 0x00; vmSector1[13] = 0x00; // HALT
    writeDisk(&hardDisk, 0, vmSector1);
    registerFile(1, 0, 1);

    // Second program LOAD R0,#3; LOAD R1,#4; ADD; HALT
    uint8_t vmSector2[DISK_SECTOR_SIZE] = {0};
    vmSector2[0] = 0x04; vmSector2[1] = 0x00; // wordCount
    vmSector2[2] = 0x00; vmSector2[3] = 0x00; // entryOffset
    vmSector2[4] = 0x00; vmSector2[5] = 0x00; // loadAddress
    vmSector2[6] = 0x03; vmSector2[7] = 0x10; // 0x1003
    vmSector2[8] = 0x04; vmSector2[9] = 0x12; // 0x1204
    vmSector2[10] = 0x40; vmSector2[11] = 0x30; // ADD
    vmSector2[12] = 0x00; vmSector2[13] = 0x00; // HALT
    writeDisk(&hardDisk, 1, vmSector2);
    registerFile(2, 1, 1);

    // Third program: READ then a couple ops so we see IO + resume
    // wordCount=8, entryOffset=0, loadAddress=0 (spans two 16-byte sectors)
    uint8_t vmSector3[DISK_SECTOR_SIZE * 2] = {0};
    vmSector3[0] = 0x08; vmSector3[1] = 0x00; // wordCount=8
    vmSector3[2] = 0x00; vmSector3[3] = 0x00; // entryOffset=0
    vmSector3[4] = 0x00; vmSector3[5] = 0x00; // loadAddress=0
    vmSector3[6]  = 0x00; vmSector3[7]  = 0x10; // 0x1000  (R0=sector0)
    vmSector3[8]  = 0x10; vmSector3[9]  = 0x12; // 0x1210  (R1=offset16)
    vmSector3[10] = 0x04; vmSector3[11] = 0x14; // 0x1404  (R2=4 words)
    vmSector3[12] = 0x00; vmSector3[13] = 0xB0; // 0xB000  (READ)
    vmSector3[14] = 0x05; vmSector3[15] = 0x10; // 0x1005  (R0=5)
    // spill remaining words into sector 3 (second sector for this program)
    vmSector3[16] = 0x06; vmSector3[17] = 0x12; // 0x1206  (R1=6)
    vmSector3[18] = 0x40; vmSector3[19] = 0x30; // 0x3040  (ADD R0,R1)
    vmSector3[20] = 0x00; vmSector3[21] = 0x00; // HALT
    writeDisk(&hardDisk, 2, vmSector3);                            // first sector
    writeDisk(&hardDisk, 3, vmSector3 + DISK_SECTOR_SIZE);         // second sector
    registerFile(3, 2, 2);

    VirtualMachine* jobVM1 = createVM(&hardDisk, &physicalMemory, 0); // empty for now
    VirtualMachine* jobVM2 = createVM(&hardDisk, &physicalMemory, 0); // empty for now
    VirtualMachine* jobVM3 = createVM(&hardDisk, &physicalMemory, 0); // empty for now
    Process* startProc = initProcess(1, true, "Start-Stop", START_STOP, NULL, (ProcessEntry)startStopEntry, 0);
    Process* idleProc = initProcess(2, true, "Idle", IDLE, NULL, (ProcessEntry)idleEntry, 0);
    Process* intrProc  = initProcess(3, true, "Interrupt-Handler", INTERRUPT_HANDLER, NULL, (ProcessEntry)interruptHandlerEntry, 0);
    Process* ioProc   = initProcess(4, true, "IO-Handler", IO_HANDLER, NULL, (ProcessEntry)ioHandlerEntry, 0);
    Process* jobProc1 = initProcess(5, true, "Governer1", JOB_GOVERNOR, jobVM1, (ProcessEntry)jobGovernorEntry, 1);
    Process* jobProc2 = initProcess(6, true, "Governer2", JOB_GOVERNOR, jobVM2, (ProcessEntry)jobGovernorEntry, 2);
    Process* jobProc3 = initProcess(7, true, "Governer3", JOB_GOVERNOR, jobVM3, (ProcessEntry)jobGovernorEntry, 3);

    addProc(scheduler, startProc);
    addProc(scheduler, idleProc);
    addProc(scheduler, intrProc);
    addProc(scheduler, ioProc);
    addProc(scheduler, jobProc1);
    addProc(scheduler, jobProc2);
    addProc(scheduler, jobProc3);

    // Disable RM demo program; let scheduler/VMs run
    realCPU.MODE = MODE_SUPERVISOR;
    realCPU.IC = 0;


    for(size_t i = 0; i < 200; ++i){
        runCycle(scheduler, &kernel);
        if (kernelStep(&kernel) == 0) break;

        //stop if no non-idle READY/BLOCKED processes remain
        bool hasWork = false;
        for (int p = SCHEDULER_MIN_PRIORITY; p < SCHEDULER_MAX_PRIORITY; ++p) {
            Process* pr = scheduler->schedule[p];
            if (!pr) continue;
            // ignore core daemons when deciding if user work remains
            if (pr->procType == IDLE || pr->procType == START_STOP ||
                pr->procType == INTERRUPT_HANDLER || pr->procType == IO_HANDLER ||
                pr->procType == LOADER) continue;
            if (pr->state == READY || pr->state == BLOCKED) { hasWork = true; break; }
        }
        if (!hasWork) break;
    }
    _log("[MAIN] OS boot stub finished\n");

    getchar();

    /*       -----------------------------------       */

    // whoever wrote these test functions has the most friction up brains in the world, not smooth.
    // testCPUProgram();
    // testProgramInterrupt();
    // testMemoryProtection();
    // testTimerInterrupt();

    // _log("\n[MAIN] All tests finished.\n");
    return 0;
}







// #include "rm/rm.h"
// #include "utils/utils.h"

// int onStartupPrompts() {
//     // do on startup commanding
//     generateConfig();

// }

// // ───────────────────────────────────────────────
// // Main entry point for testing the Real Machine
// // ───────────────────────────────────────────────
// int main(void) {
//     onStartupPrompts();
    
//     log("[MAIN] Starting Real Machine test...\n");

//     // Initialize all Real Machine components
//     initRealMachine();

//     // ───────────────────────────────
//     // Example: simple test program
//     // ───────────────────────────────
//     //
//     // Simulates a program that:
//     //   R0 = 1
//     //   R1 = 2
//     //   R0 = R0 + R1
//     //   HALT
//     //
//     // In 16-bit binary instruction format:
//     //   0x1001 = LOAD R0, #1
//     //   0x2002 = LOAD R1, #2
//     //   0x3001 = ADD  R0, R1
//     //   0x0000 = HALT

//     physicalMemory.cells[0] = 0x1001;
//     physicalMemory.cells[1] = 0x2002;
//     physicalMemory.cells[2] = 0x3001;
//     physicalMemory.cells[3] = 0x0000; // HALT

//     // Set CPU state before execution
//     realCPU.MODE = MODE_SUPERVISOR; // allow privileged instructions
//     realCPU.IC = 0;

//     log("[MAIN] Running CPU cycle...\n");
//     execCycle();

//     log("[MAIN] Done. R0=%u, R1=%u\n", realCPU.R[0], realCPU.R[1]);
//     return 0;
// }
