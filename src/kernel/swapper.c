#include "swapper.h"
#include "../utils/utils.h"
#include <string.h>

// Assumes VM_MEMORY_SIZE bytes and DISK_SECTOR_SIZE bytes per sector.
static bool writeSectorChunk(HardDisk* disk, uint16_t sector, const uint8_t* data, size_t len) {
    uint8_t buf[DISK_SECTOR_SIZE] = {0};
    size_t toCopy = len;
    if (toCopy > DISK_SECTOR_SIZE) toCopy = DISK_SECTOR_SIZE;
    memcpy(buf, data, toCopy);
    return writeDisk(disk, sector, buf);
}

static bool readSectorChunk(HardDisk* disk, uint16_t sector, uint8_t* out, size_t len) {
    uint8_t buf[DISK_SECTOR_SIZE] = {0};
    if (!readDisk(disk, sector, buf)) return false;
    size_t toCopy = len;
    if (toCopy > DISK_SECTOR_SIZE) toCopy = DISK_SECTOR_SIZE;
    memcpy(out, buf, toCopy);
    return true;
}

bool swapOutVM(HardDisk* disk, VirtualMachine* vm, uint16_t slot) {
    if (!disk || !vm || !vm->memory) return false;
    uint16_t startSector = swapSlotStart(slot);
    uint16_t sectorsNeeded = SWAP_SLOT_SIZE_SECTORS;
    if (startSector + sectorsNeeded > DISK_SECTOR_COUNT) return false;

    const uint8_t* mem = vm->memory->memoryCells;
    size_t remaining = VM_MEMORY_SIZE;
    for (uint16_t s = 0; s < sectorsNeeded && remaining > 0; ++s) {
        size_t chunk = remaining > DISK_SECTOR_SIZE ? DISK_SECTOR_SIZE : remaining;
        if (!writeSectorChunk(disk, startSector + s, mem + (s * DISK_SECTOR_SIZE), chunk)) {
            _log("[SWAP] Failed to write swap sector\n");
            return false;
        }
        remaining -= chunk;
    }
    char buf[96];
    snprintf(buf, sizeof(buf), "[SWAP] swapped OUT slot=%u bytes=%zu\n", slot, (size_t)VM_MEMORY_SIZE);
    _log(buf);
    return true;
}

bool swapInVM(HardDisk* disk, VirtualMachine* vm, uint16_t slot) {
    if (!disk || !vm || !vm->memory) return false;
    uint16_t startSector = swapSlotStart(slot);
    uint16_t sectorsNeeded = SWAP_SLOT_SIZE_SECTORS;
    if (startSector + sectorsNeeded > DISK_SECTOR_COUNT) return false;

    uint8_t* mem = vm->memory->memoryCells;
    size_t remaining = VM_MEMORY_SIZE;
    for (uint16_t s = 0; s < sectorsNeeded && remaining > 0; ++s) {
        size_t chunk = remaining > DISK_SECTOR_SIZE ? DISK_SECTOR_SIZE : remaining;
        if (!readSectorChunk(disk, startSector + s, mem + (s * DISK_SECTOR_SIZE), chunk)) {
            _log("[SWAP] Failed to read swap sector\n");
            return false;
        }
        remaining -= chunk;
    }
    char buf[96];
    snprintf(buf, sizeof(buf), "[SWAP] swapped IN slot=%u bytes=%zu\n", slot, (size_t)VM_MEMORY_SIZE);
    _log(buf);
    return true;
}
