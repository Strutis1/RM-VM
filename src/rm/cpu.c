#include "rm.h"






void initCPU(CPU *cpu){
    cpu->IC = 0;
    for(size_t i = 0; i < REG_COUNT; ++i){
        cpu->R[i] = 0;
    }
    cpu->SF = 0;
    cpu->MODE = MODE_SUPERVISOR;
    cpu->PI = PI_NONE;
    cpu->SI = SI_NONE;
    cpu->TI = TI_NONE;
    cpu->PTR = PTR_START;
}



bool interrupted(){
    return false;
}

uint16_t fetch(){
    uint16_t word;
    word = read(&physicalMemory, realCPU.IC);
    realCPU.IC++;
    return word;
}

//Theoretical binary format:
// [15:12] opcode   (4 bits)
// [11:9]  reg      (3 bits)
// [8:7]   mode     (2 bits)
// [6:0]   operand  (7 bits)

Instruction decode(uint16_t rawWord){
    Instruction result;
    result.opcode = (rawWord >> 12) & 0xF;
    result.reg = (rawWord >> 9) & 0x7;
    result.mode = (rawWord >> 7) & 0x3;
    result.operand = rawWord & 0x7F;
    return result;
}
 

void execute(Instruction inst) {
    //check mode 
    switch (inst.opcode) {
        case OP_LOAD:
            break;
        case OP_STORE:
            break;
        case OP_ADD:
            break;
        case OP_SUB:
            break;
        case OP_MUL:
            break;
        case OP_DIV:
            break;
        case OP_CMP:
            break;
        case OP_JMP:
            break;
        case OP_JZ:
            break;
        case OP_JNZ:
            break;
        case OP_READ:
            break;
        case OP_WRITE:
            break;
        case OP_HALT:
            break;
        case OP_NOP:
            break;
        case OP_SYS:
            break;
        default:
            realCPU.PI = PI_INVALID_OPCODE;
            break;
    }
}




void execCycle(){
    while(!interrupted()){
        uint16_t rawInstruction = fetch();
        Instruction nextInst = decode(rawInstruction);
    }
}