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

//started with interrupt handling but someone else started it
void raiseProgramInterrupt(uint16_t code) {
    switch (code) {
        case PI_NONE:
        case PI_INVALID_OPCODE:
        case PI_INVALID_ADDRESS:
        case PI_INVALID_REGISTER:
        case PI_DIVZERO:
        case PI_OVERFLOW:
            realCPU.PI = code;
            break;
        default:
            break;
    }
}




bool interrupted(){
    if(realCPU.PI != PI_NONE || realCPU.SI != SI_NONE || realCPU.TI != TI_NONE)
        return true;
    return false;
}

bool fetch(uint16_t *word) {
    *word = read(&physicalMemory, realCPU.IC);
    if (realCPU.PI != PI_NONE) return FAILURE;
    realCPU.IC++;
    return SUCCESS;
}


// Theoretical binary format (16-bit word):
//
// [15:12] opcode   (4 bits)   → operation code
// [11:9]  regA     (3 bits)   → primary register
// [8:6]   regB     (3 bits)   → secondary register (for reg–reg ops)
// [8:7]   mode     (2 bits)   → addressing mode (for memory/immediate ops)
// [6:0]   operand  (7 bits)   → memory address or immediate value
//
// PS:
//   - regB and (mode + operand) occupy the same bits, depending on opcode type
//   - Control-flow instructions (JMP, JZ, JNZ) use bits [8:0] for jump address
//   - Some instructions (HALT, NOP, SYS) use only the opcode field



//if failure a fault was raised
bool decode(uint16_t rawWord, Instruction *inst) {
    inst->raw = rawWord;
    inst->opcode = (rawWord >> 12) & 0xF;
    inst->regA = (rawWord >> 9) & 0x7;
    inst->mode = 0;
    inst->operand = 0;
    inst->regB = 0;

    switch (inst->opcode) {
        case OP_LOAD:
        case OP_STORE:
            inst->mode = (rawWord >> 7) & 0x3;
            inst->operand = rawWord & 0x7F;
            break;

        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_CMP:
            inst->regB = (rawWord >> 6) & 0x7;
            break;

        case OP_JMP:
        case OP_JZ:
        case OP_JNZ:
            inst->operand = rawWord & 0x1FF;   
            break;

        case OP_READ:
        case OP_WRITE:
        case OP_HALT:
        case OP_NOP:
        case OP_SYS:
            // no extra fields
            break;

        default:
            raiseProgramInterrupt(PI_INVALID_OPCODE);
            return FAILURE;
    }

    // validation
    if (inst->regA >= REG_COUNT) {
        raiseProgramInterrupt(PI_INVALID_REGISTER);
        return FAILURE;
    }

    if ((inst->opcode == OP_LOAD || inst->opcode == OP_STORE ||
         inst->opcode == OP_JMP  || inst->opcode == OP_JZ ||
         inst->opcode == OP_JNZ) &&
        inst->operand >= MEMORY_SIZE) {
        raiseProgramInterrupt(PI_INVALID_ADDRESS);
        return FAILURE;
    }

    if (inst->mode >= 4) {
        raiseProgramInterrupt(PI_INVALID_OPCODE);
        return FAILURE;
    }

    return SUCCESS;
}


// instruction handling
//define right hand values before switch maybe better
void execute(Instruction inst) {
    //check mode 
    switch (inst.opcode) {
        case OP_LOAD:
            switch (inst.mode) {
                case 0:
                    realCPU.R[inst.regA] = inst.operand;
                    break;
                case 1:
                    realCPU.R[inst.regA] = read(&physicalMemory, inst.operand);
                    break;
                case 2:
                    realCPU.R[inst.regA] =
                        read(&physicalMemory, read(&physicalMemory, inst.operand));
                    break;
                default:
                    raiseProgramInterrupt(PI_INVALID_OPCODE);
                    break;
            }
            break;
        case OP_STORE:
            switch (inst.mode) {
                // case 0:
                // invalid
                case 1:
                    write(&physicalMemory, inst.operand, realCPU.R[inst.regA]);
                    break;
                case 2:
                    write(&physicalMemory,
                          read(&physicalMemory, inst.operand),
                          realCPU.R[inst.regA]);
                    break;
                default:
                    raiseProgramInterrupt(PI_INVALID_OPCODE);
                    break;
            }
            break;
        case OP_ADD:
            realCPU.SF = 0;
            if (realCPU.R[inst.regA] > UINT16_MAX - realCPU.R[inst.regB]) {
                realCPU.SF |= FLAG_OVERFLOW | FLAG_CARRY;
                raiseProgramInterrupt(PI_OVERFLOW);
            }
            realCPU.R[inst.regA] += realCPU.R[inst.regB];
            if (realCPU.R[inst.regA] == 0) realCPU.SF |= FLAG_ZERO;
            if (realCPU.R[inst.regA] & 0x8000) realCPU.SF |= FLAG_NEGATIVE;
            break;

        case OP_SUB:
            realCPU.SF = 0;
            if (realCPU.R[inst.regA] < realCPU.R[inst.regB]) {
                realCPU.SF |= FLAG_OVERFLOW | FLAG_CARRY;
                raiseProgramInterrupt(PI_OVERFLOW);
            }
            realCPU.R[inst.regA] -= realCPU.R[inst.regB];
            if (realCPU.R[inst.regA] == 0) realCPU.SF |= FLAG_ZERO;
            if (realCPU.R[inst.regA] & 0x8000) realCPU.SF |= FLAG_NEGATIVE;
            break;

        case OP_MUL:
            realCPU.SF = 0;
            if (realCPU.R[inst.regB] != 0 &&
                realCPU.R[inst.regA] > UINT16_MAX / realCPU.R[inst.regB]) {
                realCPU.SF |= FLAG_OVERFLOW | FLAG_CARRY;
                raiseProgramInterrupt(PI_OVERFLOW);
            }
            realCPU.R[inst.regA] *= realCPU.R[inst.regB];
            if (realCPU.R[inst.regA] == 0) realCPU.SF |= FLAG_ZERO;
            if (realCPU.R[inst.regA] & 0x8000) realCPU.SF |= FLAG_NEGATIVE;
            break;

        case OP_DIV:
            realCPU.SF = 0;
            if (realCPU.R[inst.regB] == 0) {
                raiseProgramInterrupt(PI_DIVZERO);
                break;
            }
            realCPU.R[inst.regA] /= realCPU.R[inst.regB];
            if (realCPU.R[inst.regA] == 0) realCPU.SF |= FLAG_ZERO;
            if (realCPU.R[inst.regA] & 0x8000) realCPU.SF |= FLAG_NEGATIVE;
            break;

        case OP_CMP:
            realCPU.SF = 0;
            if (realCPU.R[inst.regA] == realCPU.R[inst.regB]) realCPU.SF |= FLAG_ZERO;
            else if (realCPU.R[inst.regA] < realCPU.R[inst.regB]) realCPU.SF |= FLAG_NEGATIVE;
            break;
        case OP_JMP:
            switch (inst.mode) {
                case 0:
                    realCPU.IC = inst.operand;
                    break;
                case 1:
                    realCPU.IC = read(&physicalMemory, inst.operand);
                    break;
                case 2:
                    realCPU.IC =
                        read(&physicalMemory, read(&physicalMemory, inst.operand));
                    break;
                default:
                    raiseProgramInterrupt(PI_INVALID_OPCODE);
                    break;
            }
            break;
        case OP_JZ:
            if(realCPU.SF & FLAG_ZERO){
                realCPU.IC = inst.operand;
            }
            break;
        case OP_JNZ:
            if(!(realCPU.SF & FLAG_ZERO)){
                realCPU.IC = inst.operand;
            }
            break;
        case OP_READ:
            if (realCPU.MODE != MODE_SUPERVISOR){
                raiseProgramInterrupt(PI_INVALID_OPCODE);
                break;
            }
                realCPU.SI = SI_READ;
                break;
        case OP_WRITE:
            if (realCPU.MODE != MODE_SUPERVISOR){
                raiseProgramInterrupt(PI_INVALID_OPCODE);
                break;
            }
            realCPU.SI = SI_WRITE;
            break;
        case OP_HALT:
            if (realCPU.MODE != MODE_SUPERVISOR){
                raiseProgramInterrupt(PI_INVALID_OPCODE);
                break;
            }
            realCPU.SI = SI_HALT;
            break;
        case OP_NOP:
            break;
        case OP_SYS:
            realCPU.SI = SI_SYS;
            break;
        default:
            raiseProgramInterrupt(PI_INVALID_OPCODE);
            break;
    }
}




void execCycle() {
    int running = 1;
    int ticks = 0;

    while (running) {
        while (!interrupted()) {
            uint16_t rawInstruction;
            if (!fetch(&rawInstruction)) break;

            Instruction decodedInst;
            if (!decode(rawInstruction, &decodedInst)) break;

            execute(decodedInst);

            if (++ticks >= TICK_LIMIT) {
                raiseTimerInterrupt(TI_EXPIRED);
                ticks = 0;
            }
        }

        // running = handleInterrupts();
    }
}