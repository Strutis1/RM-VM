#ifndef SWAPPER_H
#define SWAPPER_H

#include "../../include/disk.h"
#include "../vm/vm.h"


#define SWAP_START_SECTOR 100
#define SWAP_SLOT_SIZE_SECTORS 16 // 16 * 16B = 256B (covers VM_MEMORY_SIZE bytes)

static inline uint16_t swapSlotStart(uint16_t slot) {
    return (uint16_t)(SWAP_START_SECTOR + (slot * SWAP_SLOT_SIZE_SECTORS));
}

//Write the VM memory image to disk at the given slot. Returns true on success.
bool swapOutVM(HardDisk* disk, VirtualMachine* vm, uint16_t slot);

//Read the VM memory image from disk into the VM at the given slot. Returns true on success.
bool swapInVM(HardDisk* disk, VirtualMachine* vm, uint16_t slot);

#endif

