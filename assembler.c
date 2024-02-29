#include <stdio.h>
#include <stdint.h>

#define ADDRESSING_MODE_IMMEDIATE 0
#define ADDRESSING_MODE_REGISTER 1
#define ADDRESSING_MODE_DIRECT 2
#define ADDRESSING_MODE_INDIRECT 3

#define OP_MOV 0
#define OP_ADD 1

typedef struct {
    uint8_t opcode;
    uint8_t addressing_mode1;
    uint8_t addressing_mode2;
    uint64_t operand1;
    uint64_t operand2;
} instruction_t;

int main(int argc, char *argv[]){
    printf("Hello, World!\n");
    return 0;
}