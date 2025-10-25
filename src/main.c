#include "rm/rm.h"
#include <stdio.h>
#include "utils/utils.h"

int onStartupPrompts() {
    // do on startup commanding
    generateConfig();

}


int main(void) {
    onStartupPrompts();
    
    log("[MAIN] Starting Real Machine test...\n");

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

    log("[MAIN] Running CPU cycle...\n");
    execCycle();

    log("[MAIN] Done. R0=%u, R1=%u\n", realCPU.R[0], realCPU.R[1]);
    return 0;
}
