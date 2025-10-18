
#include <memory.h>
#include "../../include/common.h"
#include "../../include/disk.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"


//could chain this 4 times to generate a disk size of 4 programs spanning 1 kb
// this is a POC
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


// returns an instruction ptr housing all of the instructions
Instruction* loadProgram(uint8_t programFlag) {

    return NULL;
}