#include "vm_program.h"


uint8_t* generateProgramRegion() {
    uint8_t* programRegion = (uint8_t) malloc(MAX_PROGRAM_SIZE * sizeof(uint8_t));
    memset(programRegion, 0x00, MAX_PROGRAM_SIZE-1);
    programRegion[0] = 0xAB;
    programRegion[1] = 0xCD;
    programRegion[2] = 0xEF;

    programRegion[MAX_PROGRAM_SIZE - 3] = 0xFE;
    programRegion[MAX_PROGRAM_SIZE - 2] = 0xDC;
    programRegion[MAX_PROGRAM_SIZE - 1] = 0xBA;

    return programRegion;
}

const char* translateInstructions(Instruction* Instructions) {

    return NULL;
}