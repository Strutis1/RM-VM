#include "../../include/common.h"
#include "../../include/registers.h"
#include "../../include/interrupts.h"
#include "../../include/memory.h"
#include "../../include/timer.h"
#include "cpu.h"
#include <stdlib.h>
#include <stdbool.h>




void RMinitMemory(Memory *mem){
    for (int i = 0; i < MEMORY_SIZE; i++) {
        mem->cells[i] = 0;
    }
}

void write(Memory *mem, uint16_t address, uint16_t value){
    if(address >= USER_MEMORY_START && address <= USER_MEMORY_END){
        mem->cells[address] = value;
    }else if(address >= SUPERVISOR_MEMORY_START && address <= SUPERVISOR_MEMORY_END){
        if(realCPU.MODE == MODE_SUPERVISOR){
            mem->cells[address] = value;
        }else{
            raiseProgramInterrupt(PI_INVALID_ADDRESS);
        }
    }else{
        raiseProgramInterrupt(PI_INVALID_ADDRESS);
    }
}


uint16_t read(Memory *mem, uint16_t address){
    if(address >= USER_MEMORY_START && address <= USER_MEMORY_END){
        return mem->cells[address];
    }else if(address >= SUPERVISOR_MEMORY_START && address <= SUPERVISOR_MEMORY_END){
        if(realCPU.MODE == MODE_SUPERVISOR){
            return mem->cells[address];
        }else{
            raiseProgramInterrupt(PI_INVALID_ADDRESS);
            return 0;
        }
    }else{
        raiseProgramInterrupt(PI_INVALID_ADDRESS);
        return 0;
    }
}