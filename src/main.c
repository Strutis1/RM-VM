


#include <stdio.h>
#include "rm/rm.h"
#include "utils/utils.h"
#include "../include/interrupts.h"
#include "utils/utils.h"
#include "memory.h"



// Startup helper
static void onStartupPrompts(void) {
    generateConfig();
}

// CPU + instruction execution test
static void testCPUProgram(void) {
    _log("\n[TEST] CPU instruction execution\n");

    /*
        Program:
            LOAD R0, #1
            LOAD R1, #2
            ADD  R0, R1
            HALT
    */

    physicalMemory.cells[0] = 0x1001;
    physicalMemory.cells[1] = 0x2002;
    physicalMemory.cells[2] = 0x3001;
    physicalMemory.cells[3] = 0x0000;

    realCPU.IC = 0;
    realCPU.MODE = MODE_SUPERVISOR;

    execCycle();

    char buf[80];
    snprintf(buf, sizeof(buf), "[RESULT] R0 = %u (expected 3)\n", realCPU.R[0]);
    _log(buf);
    snprintf(buf, sizeof(buf), "[RESULT] R1 = %u (expected 2)\n", realCPU.R[1]);
    _log(buf);
}

// interrupt test
static void testProgramInterrupt(void) {
    _log("\n[TEST] Program interrupt (DIVZERO)\n");

    physicalMemory.cells[0] = 0x1001; // LOAD R0, #1
    physicalMemory.cells[1] = 0x2000; // LOAD R1, #0
    physicalMemory.cells[2] = 0x6001; // DIV R0, R1
    physicalMemory.cells[3] = 0x0000; // HALT

    realCPU.IC = 0;
    realCPU.MODE = MODE_SUPERVISOR;
    realCPU.PI = PI_NONE;

    execCycle();

    char buf[80];
    snprintf(buf, sizeof(buf), "[RESULT] PI = %u (expected %u)\n", realCPU.PI, PI_DIVZERO);
    _log(buf);
}

// MEM proc test
static void testMemoryProtection(void) {
    _log("\n[TEST] Memory protection\n");

    realCPU.MODE = MODE_USER;
    realCPU.PI = PI_NONE;

    // Attempt supervisor memory access
    read(&physicalMemory, SUPERVISOR_MEMORY_START);

    char buf[80];
    snprintf(buf, sizeof(buf), "[RESULT] PI = %u (expected %u)\n",
             realCPU.PI, PI_INVALID_ADDRESS);
    _log(buf);
}

// Timer interrupt test

static void testTimerInterrupt(void) {
    _log("\n[TEST] Timer interrupt\n");

    realCPU.TI = TI_NONE;

    for (int i = 0; i < TICK_LIMIT + 1; ++i) {
        raiseTimerInterrupt(TI_EXPIRED);
    }

    char buf[80];
    snprintf(buf, sizeof(buf), "[RESULT] TI = %u (expected %u)\n",
             realCPU.TI, TI_EXPIRED);
    _log(buf);
}

int main(void) {
    onStartupPrompts();

    _log("[MAIN] Initializing Real Machine...\n");
    initRealMachine();

    // Clear CPU state before tests
    initCPU(&realCPU);

    testCPUProgram();
    testProgramInterrupt();
    testMemoryProtection();
    testTimerInterrupt();

    _log("\n[MAIN] All tests finished.\n");
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
