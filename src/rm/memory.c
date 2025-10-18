#include "../../include/memory.h"
#include "../../include/common.h"




void RMinitMemory(Memory *mem){
    for (int i = 0; i < MEMORY_SIZE; i++) {
        mem->cells[i] = 0;
    }
}

void write(Memory *mem, uint16_t address, uint16_t value){
    //check mode here
    if(address >= MEMORY_SIZE || address < 0){
        //call interrupt maybe
    }else{
        mem->cells[address] = value;
    }
}


int read(Memory *mem, uint16_t address){
    //check mode here
    if(address > MEMORY_SIZE || address < 0){
        //call interrupt maybe
    }else{
        return mem->cells[address];
    }
}