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

void runOperations(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        _log("[VM] Invalid VM state.\n");
        return;
    }

    VM_CPU* cpu = vm->vm_cpu;
    VM_MEMORY* mem = vm->memory;
    uint16_t pc = cpu->PC;

    _log("[VM] Beginning instruction execution...\n");

    while (1) {
        // Fetch 16-bit instruction
        uint16_t raw = (mem->memoryCells[pc] << 8) | mem->memoryCells[pc + 1];
        pc += 2;

        Instruction inst;
        if (executeInstruction(&inst, raw) != 0) {
            char buf[64];
            snprintf(buf, sizeof(buf), "[VM] Invalid instruction: 0x%04X\n", raw);
            _log(buf);
            break;
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
    }

    _log("[VM] Execution finished.\n");
}
