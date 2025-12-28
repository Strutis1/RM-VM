#include "loader.h"
#include "../utils/utils.h"

#include <stddef.h>

#define LOADER_HEADER_WORDS 2
#define WORDS_PER_SECTOR (DISK_SECTOR_SIZE / 2)

static inline uint16_t read_le16(const uint8_t *buf, size_t byteIndex) {
    return (uint16_t)(buf[byteIndex] | (uint16_t)(buf[byteIndex + 1] << 8));
}

static inline bool address_fits(uint16_t loadAddress, uint16_t wordCount) {
    return (uint32_t)loadAddress + (uint32_t)wordCount <= (uint32_t)USER_MEMORY_END + 1;
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
