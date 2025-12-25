// #include "rm/rm.h"
// #include <stdio.h>
// #include "utils/utils.h"

// int onStartupPrompts() {
//     // do on startup commanding
//     generateConfig();

// }


// int main(void) {
//     onStartupPrompts();
    
//     log("[MAIN] Starting Real Machine test...\n");

//     // Initialize everything
//     initRealMachine();

//     // --- Load a simple test program into memory ---
//     // Example program:
//     // 0x1001 = LOAD R0, #1
//     // 0x2002 = LOAD R1, #2
//     // 0x3001 = ADD R0, R1
//     // 0x0000 = HALT

//     physicalMemory.cells[0] = 0x1001;
//     physicalMemory.cells[1] = 0x2002;
//     physicalMemory.cells[2] = 0x3001;
//     physicalMemory.cells[3] = 0x0000; // HALT

//     realCPU.MODE = MODE_SUPERVISOR; // allow HALT
//     realCPU.IC = 0;

//     log("[MAIN] Running CPU cycle...\n");
//     execCycle();

//     log("[MAIN] Done. R0=%u, R1=%u\n", realCPU.R[0], realCPU.R[1]);
//     return 0;
// }
#include "rm/rm.h"
#include "utils/utils.h"

int onStartupPrompts() {
    // do on startup commanding
    generateConfig();

}

// ───────────────────────────────────────────────
// Main entry point for testing the Real Machine
// ───────────────────────────────────────────────
int main(void) {
    onStartupPrompts();
    
    log("[MAIN] Starting Real Machine test...\n");

    // Initialize all Real Machine components
    initRealMachine();

    // ───────────────────────────────
    // Example: simple test program
    // ───────────────────────────────
    //
    // Simulates a program that:
    //   R0 = 1
    //   R1 = 2
    //   R0 = R0 + R1
    //   HALT
    //
    // In 16-bit binary instruction format:
    //   0x1001 = LOAD R0, #1
    //   0x2002 = LOAD R1, #2
    //   0x3001 = ADD  R0, R1
    //   0x0000 = HALT

    physicalMemory.cells[0] = 0x1001;
    physicalMemory.cells[1] = 0x2002;
    physicalMemory.cells[2] = 0x3001;
    physicalMemory.cells[3] = 0x0000; // HALT

    // Set CPU state before execution
    realCPU.MODE = MODE_SUPERVISOR; // allow privileged instructions
    realCPU.IC = 0;

    log("[MAIN] Running CPU cycle...\n");
    execCycle();

    log("[MAIN] Done. R0=%u, R1=%u\n", realCPU.R[0], realCPU.R[1]);
    return 0;
}
