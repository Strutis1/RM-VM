#include "loader.h"
#include "../utils/utils.h"
#include "scheduler.h"
#include "kernel.h"
#include "resourceManager.h"

#include <stddef.h>
#include <string.h>

#define LOADER_HEADER_WORDS 3
#define WORDS_PER_SECTOR (DISK_SECTOR_SIZE / 2)

static inline uint16_t read_le16(const uint8_t *buf, size_t byteIndex) {
    return (uint16_t)(buf[byteIndex] | (uint16_t)(buf[byteIndex + 1] << 8));
}

static inline bool address_fits(uint16_t loadAddress, uint16_t wordCount) {
    // VM memory measured in bytes; wordCount is 16-bit words
    uint32_t bytesNeeded = (uint32_t)wordCount * 2;
    return (uint32_t)loadAddress + bytesNeeded <= (uint32_t)VM_MEMORY_SIZE;
}

static FileEntry fileTable[MAX_FILE_ENTRIES];
static size_t fileCount = 0;

void initFileTable(void) {
    memset(fileTable, 0, sizeof(fileTable));
    fileCount = 0;
}

bool registerFile(uint16_t fileId, uint16_t startSector, uint16_t sectorCount) {
    if (fileCount >= MAX_FILE_ENTRIES) return false;
    fileTable[fileCount].fileId = fileId;
    fileTable[fileCount].startSector = startSector;
    fileTable[fileCount].sectorCount = sectorCount;
    fileCount++;
    return true;
}

bool findFile(uint16_t fileId, FileEntry *out) {
    for (size_t i = 0; i < fileCount; ++i) {
        if (fileTable[i].fileId == fileId) {
            if (out) *out = fileTable[i];
            return true;
        }
    }
    return false;
}

bool readFileToBuffer(HardDisk *disk, uint16_t fileId, uint8_t *dst, size_t maxBytes, size_t *outBytes) {
    if (!disk || !dst || maxBytes == 0) return false;

    FileEntry fe;
    if (!findFile(fileId, &fe)) {
        _log("[LOADER] File ID not found in table\n");
        return false;
    }

    size_t copied = 0;
    uint8_t sectorBuf[DISK_SECTOR_SIZE];
    for (uint16_t s = 0; s < fe.sectorCount && copied < maxBytes; ++s) {
        if (!readDisk(disk, fe.startSector + s, sectorBuf)) {
            _log("[LOADER] Failed to read sector for file\n");
            return false;
        }
        size_t toCopy = DISK_SECTOR_SIZE;
        if (toCopy > maxBytes - copied) toCopy = maxBytes - copied;
        memcpy(dst + copied, sectorBuf, toCopy);
        copied += toCopy;
    }

    if (outBytes) *outBytes = copied;
    return true;
}

bool loaderLoadFile(HardDisk *disk, VirtualMachine *vm, uint16_t fileId, Scheduler *sch, unsigned char waiterPid) {
    if (!disk || !vm || !vm->memory) return false;

    // Memory resource arbitration (id=2)
    Process* self = NULL;
    if (sch && sch->current) self = sch->current;
    if (self && !requestResource(&resourceManager, 2, self)) {
        self->state = BLOCKED;
        return false;
    }

    FileEntry fe;
    if (!findFile(fileId, &fe)) {
        _log("[LOADER] File ID not found in table\n");
        if (self) releaseResource(&resourceManager, 2, sch);
        return false;
    }

    uint8_t sectorBuf[DISK_SECTOR_SIZE];
    if (!readDisk(disk, fe.startSector, sectorBuf)) {
        _log("[LOADER] Failed to read initial sector for file\n");
        if (self) releaseResource(&resourceManager, 2, sch);
        return false;
    }

    ProgramHeader hdr = {
        .wordCount   = read_le16(sectorBuf, 0),
        .entryOffset = read_le16(sectorBuf, 2),
        .loadAddress = read_le16(sectorBuf, 4),
    };

    if (hdr.wordCount == 0 || !address_fits(hdr.loadAddress, hdr.wordCount)) {
        _log("[LOADER] Invalid image header\n");
        if (self) releaseResource(&resourceManager, 2, sch);
        return false;
    }
    if (hdr.entryOffset >= hdr.wordCount) {
        _log("[LOADER] Invalid image: entryOffset beyond program size\n");
        if (self) releaseResource(&resourceManager, 2, sch);
        return false;
    }

    uint16_t wordsLoaded = 0;
    size_t byteOffset = sizeof(ProgramHeader);
    uint16_t sector = fe.startSector;
    uint16_t sectorsRead = 0;

    while (wordsLoaded < hdr.wordCount && sectorsRead < fe.sectorCount) {
        // Copy words from current sector buffer
        while (byteOffset + 1 < DISK_SECTOR_SIZE && wordsLoaded < hdr.wordCount) {
            uint16_t word = read_le16(sectorBuf, byteOffset);
            uint32_t byteIndex = hdr.loadAddress + (wordsLoaded * 2);
            // VM fetch expects high byte then low byte
            if (byteIndex + 1 < VM_MEMORY_SIZE) {
                vm->memory->memoryCells[byteIndex]     = (uint8_t)(word >> 8);
                vm->memory->memoryCells[byteIndex + 1] = (uint8_t)(word & 0xFF);
            }
            wordsLoaded++;
            byteOffset += 2;
        }
        if (wordsLoaded >= hdr.wordCount) break;
        // advance to next sector
        sector++;
        sectorsRead++;
        byteOffset = 0;
        if (!readDisk(disk, sector, sectorBuf)) break;
    }

    if (wordsLoaded != hdr.wordCount) {
        _log("[LOADER] Truncated image while loading\n");
        if (self) releaseResource(&resourceManager, 2, sch);
        return false;
    }

    if (vm->vm_cpu) {
        vm->vm_cpu->PC = hdr.loadAddress + (hdr.entryOffset * 2);
        memset(vm->vm_cpu->R, 0, sizeof(vm->vm_cpu->R));
        vm->vm_cpu->SF = 0;
        vm->vm_cpu->SI = SI_NONE;
    }
    vm->trap_code = SI_NONE;
    vm->trap_r0 = vm->trap_r1 = vm->trap_r2 = 0;

    if (sch) unblockProcess(sch, waiterPid);

    char buf[96];
    snprintf(buf, sizeof(buf), "[LOADER] Loaded file %u into VM (%u words) entry=%u\n",
             fileId, wordsLoaded, hdr.loadAddress + hdr.entryOffset);
    _log(buf);

    if (self) releaseResource(&resourceManager, 2, sch);
    return true;
}

bool loadProgramImage(HardDisk *disk, Memory *mem, CPU *cpu, uint16_t startSector, uint16_t loadAddress, LoadResult *out) {
    if (!disk || !mem || !cpu) return false;
    if (!address_fits(loadAddress, 1)) return false;

    uint8_t sectorBuf[DISK_SECTOR_SIZE];
    if (!readDisk(disk, startSector, sectorBuf)) {
        _log("[LOADER] Failed to read initial sector\n");
        return false;
    }

    ProgramHeader hdr = {
        .wordCount = read_le16(sectorBuf, 0),
        .entryOffset = read_le16(sectorBuf, 2),
        .loadAddress = read_le16(sectorBuf, 4),
    };

    if (hdr.wordCount == 0) {
        _log("[LOADER] Invalid image: wordCount is zero\n");
        return false;
    }

    if (!address_fits(loadAddress, hdr.wordCount)) {
        _log("[LOADER] Image does not fit in user memory\n");
        return false;
    }

    if (hdr.entryOffset >= hdr.wordCount) {
        _log("[LOADER] Invalid image: entryOffset beyond program size\n");
        return false;
    }

    uint16_t wordsLoaded = 0;
    uint16_t sectorsUsed = 0;
    uint16_t sector = startSector;
    size_t wordIndexInSector = LOADER_HEADER_WORDS; // skip header

    while (wordsLoaded < hdr.wordCount) {
        for (; wordIndexInSector < WORDS_PER_SECTOR && wordsLoaded < hdr.wordCount; ++wordIndexInSector) {
            uint16_t word = read_le16(sectorBuf, wordIndexInSector * 2);
            writeSupervisor(mem, loadAddress + wordsLoaded, word);
            ++wordsLoaded;
        }

        if (wordsLoaded >= hdr.wordCount) break;

        // Move to next sector
        sector++;
        sectorsUsed++;
        if (!readDisk(disk, sector, sectorBuf)) {
            _log("[LOADER] Failed to read continuation sector\n");
            return false;
        }
        wordIndexInSector = 0;
    }

    sectorsUsed++; // count the first sector

    uint16_t entryPoint = loadAddress + hdr.entryOffset;
    cpu->IC = entryPoint;

    if (out) {
        out->loadAddress = loadAddress;
        out->entryPoint = entryPoint;
        out->wordsLoaded = wordsLoaded;
        out->startSector = startSector;
        out->sectorsUsed = sectorsUsed;
    }

    _log("[LOADER] Program loaded successfully\n");
    return true;
}

// Loader process entry: load fileId into proc->vm, then signal parent (Job Governor)
int loaderEntry(Process* proc, Scheduler* scheduler, Kernel* kernel, uint16_t fileId) {
    (void)kernel;
    if (!proc || !scheduler) return 0;

    // find parent JG and its VM
    Process* parent = NULL;
    for (int i = SCHEDULER_MIN_PRIORITY; i < SCHEDULER_MAX_PRIORITY; ++i) {
        if (scheduler->schedule[i] && scheduler->schedule[i]->pid == proc->parentPid) {
            parent = scheduler->schedule[i];
            break;
        }
    }
    if (!parent || !parent->vm) return 0;

    if (!loaderLoadFile(&hardDisk, parent->vm, fileId, scheduler, parent->pid)) {
        _log("[LOADER] Loader process failed\n");
        return 0; // finish
    }
    // mark parent as loaded
    parent->loaded = 1;
    parent->state = READY;
    _log("[LOADER] Loader process done\n");
    return 0; // finish
}
