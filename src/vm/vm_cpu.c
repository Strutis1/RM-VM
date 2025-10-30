#include "vm_cpu.h"
#include "vm.h"

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