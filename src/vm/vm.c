// #include "vm.h"
// #include <stdlib.h>

// /*
// typedef struct {
//     Instruction* instructions;
//     Channel* channel;
//     VM_MEMORY* memory;
//     VM_CPU* vm_cpu;
// } VirtualMachine;
// */

// VirtualMachine* createVM(Channel* channel) {
//     if (!channel) return NULL;
//     uint64_t* bin = readChannel(channel);
//     if (!bin || !bin[0]) return NULL;
    
//     int n = sizeof(bin) / sizeof(uint64_t);
//     Instruction* ins = (Instruction*)malloc(n * sizeof(Instruction));
//     for (int i = 0; i < n; ++i) {
//         char ans = stuffInstructions(ins + i, bin[i]);
//         if (ans != 0) {
//             free(ins);
//             return NULL;
//         }
//     }

//     VirtualMachine* machine = { ins, initChannel(), VMinitMemory(), initCPU() };
    
//     if (loadProgram(ins, machine->memory) != 0) return NULL; 

//     runOperations(machine);

//     return NULL;
// }

#include "vm.h"
#include "vm_channel.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

VirtualMachine* createVM(HardDisk* disk, Memory* rmMemory) {
    if (!disk || !rmMemory) {
        printf("[VM] Invalid disk or RM memory pointer.\n");
        return NULL;
    }


    VirtualMachine* vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    if (!vm) return NULL;

    memset(vm, 0, sizeof(VirtualMachine));

    vm->memory = VMinitMemory();
    vm->vm_cpu = initVMCPU();
    VMinitChannel(&vm->channel);

    if (!vm->memory || !vm->vm_cpu) {
        printf("[VM] Memory or CPU initialization failed.\n");
        free(vm);
        return NULL;
    }

    printf("[VM] Loading program from disk into VM memory...\n");

    // Load first few sectors into VM memory as program
    uint8_t buffer[DISK_SECTOR_SIZE];
    if (!readDisk(disk, 0, buffer)) {
        printf("[VM] Failed to read from disk.\n");
        destroyVM(vm);
        return NULL;
    }

    // Copy data to VM memory (simple direct copy)
    memcpy(vm->memory->memoryCells, buffer, DISK_SECTOR_SIZE);
    printf("[VM] Program loaded successfully.\n");

    return vm;
}

void destroyVM(VirtualMachine* vm) {
    if (!vm) return;
    if (vm->instructions) free(vm->instructions);
    if (vm->memory) free(vm->memory);
    if (vm->vm_cpu) free(vm->vm_cpu);
    free(vm);
    printf("[VM] Destroyed.\n");
}

void runVM(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        printf("[VM] Invalid VM state.\n");
        return;
    }

    printf("[VM] Starting execution...\n");

    // Simple pseudo loop for demonstration
    uint16_t pc = 0;
    for (int tick = 0; tick < 20; ++tick) { // limit ticks for safety
        uint16_t raw = (vm->memory->memoryCells[pc] << 8) |
                       (vm->memory->memoryCells[pc + 1]);
        pc += 2;

        Instruction inst;
        if (executeInstruction(&inst, raw) != 0) {
            printf("[VM] Invalid instruction at %u (0x%04X)\n", pc, raw);
            break;
        }

        printf("[VM] Executing opcode 0x%X (PC=%u)\n", inst.opcode, pc);

        if (inst.opcode == OP_HALT) {
            printf("[VM] HALT reached. Stopping.\n");
            break;
        }
    }

    printf("[VM] Execution complete.\n");
}

void loadDemoProgram(void) {
    // Simple binary instruction stream for the VM
    uint16_t program[] = {
        0x100A, // LOAD R0, 10
        0x2100, // ADD R0, R1
        0xE000  // HALT
    };

    uint8_t buffer[DISK_SECTOR_SIZE] = {0};
    memcpy(buffer, program, sizeof(program));
    writeDisk(&hardDisk, 0, buffer);

    printf("[RM] Demo program written to disk sector 0.\n");
}
