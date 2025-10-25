#include "file.h"
#include <stdio.h>
#include "instruction.h"

// let me cook...
uint8_t* generateNULLregion(int size) {
    uint8_t* datasegReg = (uint8_t) malloc(size * sizeof(uint8_t));
    memset(datasegReg, 0x00, size - 1);
    return datasegReg;
}

uint8_t* generateProgramRegion() {
    uint8_t* programRegion = (uint8_t*) malloc(MAX_PROGRAM_SIZE * sizeof(uint8_t));
    memset(programRegion, 0x00, MAX_PROGRAM_SIZE-1);
    programRegion[0] = 0xAB;
    programRegion[1] = 0xCD;
    programRegion[2] = 0xEF;

    programRegion[MAX_PROGRAM_SIZE - 3] = 0xFE;
    programRegion[MAX_PROGRAM_SIZE - 2] = 0xDC;
    programRegion[MAX_PROGRAM_SIZE - 1] = 0xBA;

    return programRegion;
}

uint8_t* generateBinBinary() {
    uint8_t* dseg = generateNULLregion(DATASEG_SIZE);
    uint8_t* pseg = generateProgramRegion();
    uint8_t* aseg = generateNULLregion(ANSWER_REGION);

    int maxMem = 256;

    uint8_t* storee = (uint8_t*) malloc(maxMem * sizeof(uint8_t));
    memmove(storee, dseg, DATASEG_SIZE);
    memmove(storee + DATASEG_SIZE, pseg, MAX_PROGRAM_SIZE);
    memmove(storee + DATASEG_SIZE + MAX_PROGRAM_SIZE, aseg, ANSWER_REGION);
    memset(storee + maxMem, 0x00, 1);

    free(dseg);
    free(pseg);
    free(aseg);

    return storee;
}

char putToBin(const char* _fname, uint8_t* binary) {
    char* fname = (char*) malloc(sizeof(_fname));
    memcpy(fname, _fname, sizeof(_fname));
    memset(fname + sizeof(_fname) - 4, "bin\0", 4);

    FILE* fptr = fopen(fname, "wb");
    if (!fname) {
        printf("[ERROR] code 1");
        return 1;
    }
    
    if (translate(&binary, _fname) != 0) {
        printf("[ERROR] code 2");
        return 2;
    }
    
    fwrite(binary, 1, 256, fptr);

    fclose(fptr);
    return 0;
}

char compile(const char* _fname) {
    return putToBin(_fname, generateBinBinary());
}