#include "mmu.h"

uint16_t mmuTranslate(uint16_t virtualAddr, CPU* cpu, Memory* mem) {
    uint8_t page = virtualAddr >> 8;
    uint8_t offset = virtualAddr & 0xFF;
    uint16_t ptBase = cpu->PTR;

    uint16_t pteAddr = ptBase + page * sizeof(PageTableEntry);
    PageTableEntry pte;

    pte = *(PageTableEntry*)&mem->cells[pteAddr];

    if (!pte.present) {
        cpu->PI = PI_INVALID_ADDRESS;
        return INVALID_PAGE;
    }

    return (pte.frame * MMU_PAGE_SIZE) + offset;
}

void initPageTable(CPU* cpu, Memory* mem, uint16_t ptAddr) {
    cpu->PTR = ptAddr;

    PageTableEntry* pt = (PageTableEntry*)&mem->cells[ptAddr];

    for (int i = 0; i < MMU_PAGE_COUNT; ++i) {
        pt[i].frame = i;
        pt[i].present = true;
        pt[i].writable = true;
    }
}
