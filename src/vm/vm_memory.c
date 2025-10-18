#include "vm_memory.h"

#include <memory.h>
#include "../../include/common.h"
#include "../../include/disk.h"
#include "../../include/instructions.h"
#include "../../include/interrupts.h"


VM_MEMORY* VMinitMemory() {
    VM_MEMORY* abc;
    memset(abc->memoryCells, 0x00, VM_MEMORY_SIZE);
    return abc;
}