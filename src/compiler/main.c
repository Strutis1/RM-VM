#include "instruction.h"
#include "file.h"
#include "defines.h"


int main(const int argc, const char** argv) {
    if (!argv[1]) return 1;    
    return compile(argv[1]) == 0 ? 0 : 1;
}