#include "rm/rm.h"
#include "vm/vm.h"






int main(int argc, char *argv[]) {
    initRealMachine();

    if (argc < 2) {
        printf("Usage: program_name <program_folder>\n");
        return 1;
    }

    VirtualMachine vm = createVM();
    loadProgram(&vm, argv[1]);
    runVM(&vm);          
}       
