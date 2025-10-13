#include "../../include/memory.h"
#include "../../include/common.h"




void init(){
    
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