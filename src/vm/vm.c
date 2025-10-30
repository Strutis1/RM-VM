#include "vm.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/disk.h"

VirtualMachine* createVM(Channel* channel) {
    if (!channel) return NULL;

    VirtualMachine* vm = (VirtualMachine*)calloc(1, sizeof(VirtualMachine));
    if (!vm) return NULL;

    vm->channel = channel;
    vm->memory  = VMinitMemory();
    vm->vm_cpu = initCPU();

    if (!vm->memory || !vm->vm_cpu) {
        free(vm);
        return NULL;
    }

    uint64_t* bin = readChannel(channel);
    if (!bin) { free(vm); return NULL; }

    memcpy(vm->memory->memoryCells, bin, DISK_SECTOR_SIZE);

    runOperations(vm);

    return vm;
}
