#include "rm/rm.h"
#include <stdio.h>
#include "utils/utils.h"

void onStartupPrompts() {
    // do on startup commanding
    generateConfig();
}


int main(void) {
    onStartupPrompts();
    
    _log("[MAIN] Starting Real Machine test...\n");

    // Initialize everything
    initRealMachine();

    // --- Load a simple test program into memory ---
    // Example program:
    // 0x1001 = LOAD R0, #1
    // 0x2002 = LOAD R1, #2
    // 0x3001 = ADD R0, R1
    // 0x0000 = HALT

    physicalMemory.cells[0] = 0x1001;
    physicalMemory.cells[1] = 0x2002;
    physicalMemory.cells[2] = 0x3001;
    physicalMemory.cells[3] = 0x0000; // HALT

    realCPU.MODE = MODE_SUPERVISOR; // allow HALT
    realCPU.IC = 0;

    _log("[MAIN] Running CPU cycle...\n");
    execCycle();

    _log("[MAIN] Done.\n");
    return 0;
}
