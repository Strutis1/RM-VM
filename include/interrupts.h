#pragma once



// Interrupts
#define PI_NONE             0
#define PI_INVALID_OPCODE   1
#define PI_INVALID_ADDRESS  2
#define PI_INVALID_REGISTER 3
#define PI_DIVZERO          4
#define PI_OVERFLOW         5


#define SI_NONE             0
#define SI_READ             1
#define SI_WRITE            2
#define SI_HALT             3
#define SI_SYS              4

#define TI_NONE             0
#define TI_EXPIRED          1