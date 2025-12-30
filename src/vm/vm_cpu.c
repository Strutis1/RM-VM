#include "vm_cpu.h"
#include "vm.h"
#include "vm_memory.h"
#include "vm_program.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>

VM_CPU* initVM_CPU() {
    //VM_CPU* abc = {0, 0, 0, 0, 0, 0, 0};
    VM_CPU* abc = malloc(sizeof(VM_CPU));
    if (!abc) return NULL;
    memset(abc, 0, sizeof(VM_CPU));
    return abc;
}

void runInstruction(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        _log("[VM] Invalid VM state.\n");
        return;
    }

    VM_CPU* cpu = vm->vm_cpu;
    VM_MEMORY* mem = vm->memory;
    uint16_t pc = cpu->PC;

    if (pc + 1 >= VM_MEMORY_SIZE) {
        _log("[VM] PC out of bounds.\n");
        return;
    }

    _log("[VM] Beginning instruction execution...\n");

    
    // Fetch 16-bit instruction
    uint16_t raw = (mem->memoryCells[pc] << 8) | mem->memoryCells[pc + 1];
    cpu->PC = pc + 2; // advance for sequential execution

    Instruction inst;
    if (executeInstruction(&inst, raw) != 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[VM] Invalid instruction: 0x%04X\n", raw);
        _log(buf);
        return;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "[VM] Executing opcode %u at PC=%u\n", inst.opcode, pc);
    _log(buf);

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
            _log("[VM] HALT received.\n");
            return;

        default:
            snprintf(buf, sizeof(buf), "[VM] Unimplemented opcode %u.\n", inst.opcode);
            _log(buf);
            break;
    }
    _log("[VM] Execution finished.\n");
}


int runOperations(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        _log("[VM] Invalid VM state.\n");
        return -1;
    }

    VM_CPU* cpu = vm->vm_cpu;
    VM_MEMORY* mem = vm->memory;

    vm->trap_code = SI_NONE;
    vm->trap_r0 = vm->trap_r1 = vm->trap_r2 = 0;

    _log("[VM] Beginning instruction execution...\n");

    while (1) {
        uint16_t pc = cpu->PC;
        if (pc + 1 >= VM_MEMORY_SIZE) {
            _log("[VM] PC out of bounds.\n");
            return -1;
        }

        // Fetch 16-bit instruction
        uint16_t raw = (mem->memoryCells[pc] << 8) | mem->memoryCells[pc + 1];
        cpu->PC = pc + 2; // advance for sequential execution

        Instruction inst;
        if (executeInstruction(&inst, raw) != 0) {
            char buf[64];
            snprintf(buf, sizeof(buf), "[VM] Invalid instruction: 0x%04X\n", raw);
            _log(buf);
            return -1;
        }

        char buf[64];
        snprintf(buf, sizeof(buf), "[VM] Executing opcode %u at PC=%u\n", inst.opcode, pc);
        _log(buf);

        switch (inst.opcode) {
            case OP_LOAD:
                cpu->R[inst.regA] = inst.operand;
                break;

            case OP_READ:
                // Validate user buffer bounds before trapping to host
                if ((uint32_t)cpu->R[1] + ((uint32_t)cpu->R[2] * 2) > VM_MEMORY_SIZE) {
                    _log("[VM] READ out of bounds\n");
                    return -1;
                }
                vm->trap_code = SI_READ;
                vm->trap_r0 = cpu->R[0];
                vm->trap_r1 = cpu->R[1];
                vm->trap_r2 = cpu->R[2];
                return 2; // trap to kernel

            case OP_WRITE:
                if ((uint32_t)cpu->R[1] + ((uint32_t)cpu->R[2] * 2) > VM_MEMORY_SIZE) {
                    _log("[VM] WRITE out of bounds\n");
                    return -1;
                }
                vm->trap_code = SI_WRITE;
                vm->trap_r0 = cpu->R[0];
                vm->trap_r1 = cpu->R[1];
                vm->trap_r2 = cpu->R[2];
                return 2; // trap to kernel

            case OP_SYS:
                vm->trap_code = SI_SYS;
                vm->trap_r0 = cpu->R[0];
                vm->trap_r1 = cpu->R[1];
                vm->trap_r2 = cpu->R[2];
                return 2; // generic system call trap

            case OP_ADD:
                cpu->R[inst.regA] += cpu->R[inst.regB];
                break;

            case OP_SUB:
                cpu->R[inst.regA] -= cpu->R[inst.regB];
                break;

            case OP_HALT:
                _log("[VM] HALT received.\n");
                {
                    char summary[96];
                    snprintf(summary, sizeof(summary), "[VM] Final regs: R0=%u R1=%u R2=%u PC=%u\n",
                             cpu->R[0], cpu->R[1], cpu->R[2], cpu->PC);
                    _log(summary);
                }
                return 1;

            default:
                snprintf(buf, sizeof(buf), "[VM] Unimplemented opcode %u.\n", inst.opcode);
                _log(buf);
                return -1;
        }
    }

    _log("[VM] Execution finished.\n");
    return 0;
}
