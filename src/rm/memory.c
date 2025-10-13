#include "../../include/memory.h"



void write(Memory *mem, int address, int value){
    //check mode here
    if(address > MEMORY_SIZE || address < 0){
        //call interrupt maybe
    }else{
        mem->cells[address] = value;
    }
}


int read(Memory *mem, int address){
    //check mode here
    if(address > MEMORY_SIZE || address < 0){
        //call interrupt maybe
    }else{
        return mem->cells[address];
    }
}