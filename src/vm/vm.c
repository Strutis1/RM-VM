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
#include "vm_program.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t loadDiskProgram(HardDisk* disk, VM_MEMORY* mem, uint16_t startSector) {
    if (!disk || !mem) return 0;

    size_t offset = 0;
    uint8_t buffer[DISK_SECTOR_SIZE];

    for (uint16_t sector = startSector;
         sector < DISK_SECTOR_COUNT && offset < VM_MEMORY_SIZE;
         ++sector) {
        if (!readDisk(disk, sector, buffer)) break;

        size_t copyLen = VM_MEMORY_SIZE - offset;
        if (copyLen > DISK_SECTOR_SIZE) copyLen = DISK_SECTOR_SIZE;

        memcpy(mem->memoryCells + offset, buffer, copyLen);
        offset += copyLen;

        if (copyLen < DISK_SECTOR_SIZE) break; // memory full
    }

    return offset;
}

VirtualMachine* createVM(HardDisk* disk, Memory* rmMemory) {
    if (!disk || !rmMemory) {
        _log("[VM] Invalid disk or RM memory pointer.\n");
        return NULL;
    }
    (void)rmMemory; // rmMemory reserved for future integration with RM memory


    VirtualMachine* vm = (VirtualMachine*)calloc(1, sizeof(VirtualMachine));
    if (!vm) return NULL;


    vm->memory  = VMinitMemory();
    vm->vm_cpu  = initVM_CPU();
    vm->channel = (ChannelDevice*)calloc(1, sizeof(ChannelDevice));
    if (vm->channel) VMinitChannel(vm->channel);

    if (!vm->memory || !vm->vm_cpu || !vm->channel) {
        _log("[VM] Memory, CPU, or channel initialization failed.\n");
        destroyVM(vm);
        return NULL;
    }

    _log("[VM] Loading program from disk into VM memory...\n");

    size_t loaded = loadDiskProgram(disk, vm->memory, 0);
    if (loaded == 0) {
        _log("[VM] Failed to load any bytes from disk.\n");
        destroyVM(vm);
        return NULL;
    }

    vm->vm_cpu->PC = 0;

    char buf[64];
    snprintf(buf, sizeof(buf), "[VM] Loaded %zu bytes into VM memory.\n", loaded);
    _log(buf);

    return vm;
}

void destroyVM(VirtualMachine* vm) {
    if (!vm) return;
    if (vm->instructions) free(vm->instructions);
    if (vm->memory) free(vm->memory);
    if (vm->vm_cpu) free(vm->vm_cpu);
    if (vm->channel) free(vm->channel);
    free(vm);
    _log("[VM] Destroyed.\n");
}

void runVM(VirtualMachine* vm) {
    if (!vm || !vm->memory || !vm->vm_cpu) {
        _log("[VM] Invalid VM state.\n");
        return;
    }

    _log("[VM] Starting execution...\n");

    // Simple pseudo loop for demonstration
    uint16_t pc = 0;
    for (int tick = 0; tick < 20; ++tick) { // limit ticks for safety
        uint16_t raw = (vm->memory->memoryCells[pc] << 8) |
                       (vm->memory->memoryCells[pc + 1]);
        pc += 2;

        Instruction inst;
        if (executeInstruction(&inst, raw) != 0) {
            char buf[80];
            snprintf(buf, sizeof(buf), "[VM] Invalid instruction at %u (0x%04X)\n", pc, raw);
            _log(buf);
            break;
        }

        char buf[80];
        snprintf(buf, sizeof(buf), "[VM] Executing opcode 0x%X (PC=%u)\n", inst.opcode, pc);
        _log(buf);

        if (inst.opcode == OP_HALT) {
            _log("[VM] HALT reached. Stopping.\n");
            break;
        }
    }

    _log("[VM] Execution complete.\n");
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
   // writeDisk(&hardDisk, 0, buffer); //I think this breaks something

    _log("[RM] Demo program written to disk sector 0.\n");
}
// =======
// #include "vm.h"
// #include <stdlib.h>
// #include <string.h>
// #include "../../include/disk.h"

// VirtualMachine* createVM(Channel* channel) {
//     if (!channel) return NULL;

//     VirtualMachine* vm = (VirtualMachine*)calloc(1, sizeof(VirtualMachine));
//     if (!vm) return NULL;

//     vm->channel = channel;
//     vm->memory  = VMinitMemory();
//     vm->vm_cpu = initCPU();

//     if (!vm->memory || !vm->vm_cpu) {
//         free(vm);
//         return NULL;
//     }

//     uint64_t* bin = readChannel(channel);
//     if (!bin) { free(vm); return NULL; }

//     memcpy(vm->memory->memoryCells, bin, DISK_SECTOR_SIZE);

//     runOperations(vm);

//     return vm;
// >>>>>>> 4104e68847041b9d8d6f1b8c3de28571f2adc05c
// }
