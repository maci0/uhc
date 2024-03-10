#ifndef ISA_H
#define ISA_H

#include <stdint.h>

#define OPCODE_WIDTH 1
#define AM_WIDTH 1
#define OPERAND_WIDTH 8

#define INSTRUCTION_WIDTH (OPCODE_WIDTH + AM_WIDTH + AM_WIDTH + OPERAND_WIDTH + OPERAND_WIDTH)

typedef enum
{
    OP_NONE,
    OP_NOP,
    OP_MOV,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_JMP = 0x0F,
    OP_CMP = 0x10,
    OP_JEQ = 0x11,
    OP_CALL = 200,
    OP_RET = 201,
    OP_RST = 254,
    OP_HLT = 255,
} Opcode;

typedef enum
{
    AM_NONE = 0,
    AM_IMMEDIATE = 1,
    AM_REGISTER = 2,
} AddressingMode;




typedef struct {
    uint8_t opcode;
    uint8_t srcMode;
    uint8_t destMode;
    uint64_t srcOperand;
    uint64_t destOperand;
} Instruction ;

#endif // ISA_H