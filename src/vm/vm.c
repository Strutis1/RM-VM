#include "vm.h"

/*
typedef struct {
    Instruction* instructions;
    Channel* channel;
    VM_MEMORY* memory;
    VM_CPU* vm_cpu;
} VirtualMachine;
*/

VirtualMachine* createVM(Instruction* Instructions) {
    VirtualMachine* abc = {Instructions, initChannel(), VMinitMemory(), initCPU() };

    if (loadProgram(Instructions)) {
        abc->vm_cpu->SI = 1; // initiate halt
        abc->channel->interruptFlag = 1; //temp val
    }
    return NULL;
}

char loadProgram(Instruction* Instructions) {

}