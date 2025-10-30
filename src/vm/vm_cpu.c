#include "vm_cpu.h"
#include "vm.h"
#include "vm_memory.h"
#include <stdlib.h>
#include <string.h>

VM_CPU* initVMCPU() {
    //VM_CPU* abc = {0, 0, 0, 0, 0, 0, 0};
    VM_CPU* abc = malloc(sizeof(VM_CPU));
    if (!abc) return NULL;
    memset(abc, 0, sizeof(VM_CPU));
    return abc;
}

void runOperations(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        printf("[VM] Invalid VM state.\n");
        return;
    }

    VM_CPU* cpu = vm->vm_cpu;
    VM_MEMORY* mem = vm->memory;
    uint16_t pc = cpu->PC;

    printf("[VM] Beginning instruction execution...\n");

    while (true) {
        if (pc + 1 >= VM_MEMORY_SIZE) {
            printf("[VM] Program counter out of range.\n");
            break;
        }

        // Fetch 16-bit instruction
        uint16_t raw = (mem->memoryCells[pc] << 8) | mem->memoryCells[pc + 1];
        pc += 2;

        Instruction inst;
        if (executeInstruction(&inst, raw) != 0) {
            printf("[VM] Invalid instruction: 0x%04X\n", raw);
            break;
        }

        printf("[VM] Executing opcode %u at PC=%u\n", inst.opcode, pc);

        switch (inst.opcode) {
            case OP_LOAD:
                cpu->R[inst.regA] = inst.operand;
                break;

            case OP_ADD:
                cpu->R[inst.regA] += cpu->R[inst.regB];
                break;

            case OP_SUB:
                cpu->R[inst.regA] -= cpu->R[inst.regB];
                break;

            case OP_HALT:
                printf("[VM] HALT received.\n");
                return;

            default:
                printf("[VM] Unimplemented opcode %u.\n", inst.opcode);
                break;
        }
    }

    printf("[VM] Execution finished.\n");
}
// =======
// static inline void sf_set(VM_CPU* c, uint16_t v){
//     c->SF = 0;
//     if (v == 0) c->SF |= FLAG_ZERO;
//     if (v & 0x8000) c->SF |= FLAG_NEGATIVE;
// }

// static inline int fetch(const VirtualMachine* vm, Instruction* ins){
//     const VM_MEMORY* mem = vm->memory;
//     const VM_CPU* cpu = vm->vm_cpu;
//     if (cpu->PC + 9 > VM_MEMORY_SIZE) return -1;
//     const uint8_t* p = &mem->memoryCells[cpu->PC];
//     ins->opcode  = p[0];
//     ins->regA    = p[1];
//     ins->regB    = p[2];
//     ins->mode    = p[3];
//     ins->operand = (uint16_t)((p[4] << 8) | p[5]);
//     ins->raw     = (uint16_t)((p[6] << 8) | p[7]);
//     ins->length  = p[8];
//     return 0;
// }

// static inline uint16_t vm_mem_read16(const VirtualMachine* vm, uint16_t addr){
//     if (addr + 1 >= VM_MEMORY_SIZE) return 0;
//     const uint8_t* m = vm->memory->memoryCells;
//     return (uint16_t)((m[addr] << 8) | m[addr+1]);
// }

// static inline void vm_mem_write16(VirtualMachine* vm, uint16_t addr, uint16_t v){
//     if (addr + 1 >= VM_MEMORY_SIZE) return;
//     uint8_t* m = vm->memory->memoryCells;
//     m[addr]   = (uint8_t)(v >> 8);
//     m[addr+1] = (uint8_t)(v & 0xFF);
// }

// VM_CPU* initCPU() {
//     VM_CPU* c = (VM_CPU*)calloc(1, sizeof(VM_CPU));
//     return c;
// }

// void runOperations(VirtualMachine* vm){
//     if (!vm || !vm->vm_cpu || !vm->memory) return;
//     VM_CPU* c = vm->vm_cpu;

//     for(;;){
//         Instruction ins;
//         if (fetch(vm, &ins) != 0){ raiseProgramInterrupt(PI_INVALID_ADDRESS); break; }

//         switch(ins.opcode){
//             case OP_HALT:
//                 raiseSystemInterrupt(SI_HALT);
//                 return;

//             case OP_NOP:
//                 c->PC += 9;
//                 break;

//             case OP_LOAD: {
//                 uint16_t v = 0;
//                 switch(ins.mode & 0x3){
//                     case 0: v = ins.operand; break;
//                     case 1: v = vm_mem_read16(vm, ins.operand); break;
//                     case 2: v = c->R[ins.regB % REG_COUNT]; break;
//                     case 3: v = vm_mem_read16(vm, (uint16_t)(c->R[ins.regB % REG_COUNT] + ins.operand)); break;
//                     default: raiseProgramInterrupt(PI_INVALID_OPCODE); return;
//                 }
//                 c->R[ins.regA % REG_COUNT] = v;
//                 sf_set(c, v);
//                 c->PC += 9;
//             } break;

//             case OP_STORE: {
//                 uint16_t src = c->R[ins.regA % REG_COUNT];
//                 switch(ins.mode & 0x3){
//                     case 1: vm_mem_write16(vm, ins.operand, src); break;
//                     case 3: vm_mem_write16(vm, (uint16_t)(c->R[ins.regB % REG_COUNT] + ins.operand), src); break;
//                     default: raiseProgramInterrupt(PI_INVALID_OPCODE); return;
//                 }
//                 sf_set(c, src);
//                 c->PC += 9;
//             } break;

//             case OP_ADD: {
//                 uint16_t a = c->R[ins.regA % REG_COUNT];
//                 uint16_t b = c->R[ins.regB % REG_COUNT];
//                 uint32_t r = (uint32_t)a + (uint32_t)b;
//                 c->R[ins.regA % REG_COUNT] = (uint16_t)r;
//                 sf_set(c, (uint16_t)r);
//                 if (r > 0xFFFF) c->SF |= FLAG_CARRY;
//                 c->PC += 9;
//             } break;

//             case OP_SUB: {
//                 uint16_t a = c->R[ins.regA % REG_COUNT];
//                 uint16_t b = c->R[ins.regB % REG_COUNT];
//                 uint32_t r = (uint32_t)a - (uint32_t)b;
//                 c->R[ins.regA % REG_COUNT] = (uint16_t)r;
//                 sf_set(c, (uint16_t)r);
//                 if (a < b) c->SF |= FLAG_CARRY;
//                 c->PC += 9;
//             } break;

//             case OP_MUL: {
//                 uint32_t r = (uint32_t)c->R[ins.regA % REG_COUNT] * (uint32_t)c->R[ins.regB % REG_COUNT];
//                 c->R[ins.regA % REG_COUNT] = (uint16_t)r;
//                 sf_set(c, (uint16_t)r);
//                 if (r > 0xFFFF) c->SF |= FLAG_OVERFLOW;
//                 c->PC += 9;
//             } break;

//             case OP_DIV: {
//                 uint16_t b = c->R[ins.regB % REG_COUNT];
//                 if (b == 0){ raiseProgramInterrupt(PI_DIVZERO); return; }
//                 uint16_t r = (uint16_t)(c->R[ins.regA % REG_COUNT] / b);
//                 c->R[ins.regA % REG_COUNT] = r;
//                 sf_set(c, r);
//                 c->PC += 9;
//             } break;

//             case OP_CMP: {
//                 uint16_t a = c->R[ins.regA % REG_COUNT];
//                 uint16_t b = c->R[ins.regB % REG_COUNT];
//                 uint16_t r = (uint16_t)(a - b);
//                 sf_set(c, r);
//                 c->PC += 9;
//             } break;

//             case OP_JMP:
//                 c->PC = (uint16_t)(ins.operand * 9);
//                 break;

//             case OP_JZ:
//                 if (c->SF & FLAG_ZERO) c->PC = (uint16_t)(ins.operand * 9);
//                 else c->PC += 9;
//                 break;

//             case OP_JNZ:
//                 if (!(c->SF & FLAG_ZERO)) c->PC = (uint16_t)(ins.operand * 9);
//                 else c->PC += 9;
//                 break;

//             case OP_READ:
//                 raiseSystemInterrupt(SI_READ);
//                 return;

//             case OP_WRITE:
//                 raiseSystemInterrupt(SI_WRITE);
//                 return;

//             case OP_SYS:
//                 raiseSystemInterrupt(SI_SYS);
//                 return;

//             default:
//                 raiseProgramInterrupt(PI_INVALID_OPCODE);
//                 return;
//         }
//     }
// }
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c
