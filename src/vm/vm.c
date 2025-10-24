#include "vm.h"
#include <stdlib.h>

/*
typedef struct {
    Instruction* instructions;
    Channel* channel;
    VM_MEMORY* memory;
    VM_CPU* vm_cpu;
} VirtualMachine;
*/

VirtualMachine* createVM(Channel* channel) {
    if (!channel) return NULL;
    uint64_t* bin = readChannel(channel);
    if (!bin || !bin[0]) return NULL;
    
    int n = sizeof(bin) / sizeof(uint64_t);
    Instruction* ins = (Instruction*)malloc(n * sizeof(Instruction));
    for (int i = 0; i < n; ++i) {
        char ans = stuffInstructions(ins + i, bin[i]);
        if (ans != 0) {
            free(ins);
            return NULL;
        }
    }

    VirtualMachine* machine = { ins, initChannel(), VMinitMemory(), initCPU() };
    
    if (loadProgram(ins, machine->memory) != 0) return NULL; 

    runOperations(machine);

    return NULL;
}