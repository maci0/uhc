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
    
    OP_LDR = 210,
    OP_STR = 211,
    OP_RST = 254,
    OP_HLT = 255,
} Opcode;

typedef enum
{
    AM_NONE = 0,
    AM_IMMEDIATE = 1,
    AM_REGISTER = 2,
    AM_DIRECT = 4,
} AddressingMode;

typedef struct {
    uint8_t opcode;
    uint8_t srcMode;
    uint8_t destMode;
    uint64_t srcOperand;
    uint64_t destOperand;
} Instruction ;

Instruction instructionSet[256] = {
    [OP_NOP] = {.opcode = OP_NOP, .srcMode = AM_NONE, .destMode = AM_NONE, .srcOperand = false, .destOperand = false},
    [OP_MOV] = {.opcode = OP_MOV, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_PUSH] = {.opcode = OP_PUSH, .srcMode = AM_REGISTER, .destMode = AM_NONE, .srcOperand = true, .destOperand = false},
    [OP_PUSH] = {.opcode = OP_PUSH, .srcMode = AM_NONE, .destMode = AM_REGISTER, .srcOperand = false, .destOperand = true},
    [OP_ADD] = {.opcode = OP_ADD, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_SUB] = {.opcode = OP_SUB, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_MUL] = {.opcode = OP_MUL, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_DIV] = {.opcode = OP_DIV, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_CALL] = {.opcode = OP_CALL, .srcMode = AM_NONE, .destMode = AM_IMMEDIATE, .srcOperand = false, .destOperand = true},
    [OP_JMP] = {.opcode = OP_JMP, .srcMode = AM_NONE, .destMode = AM_IMMEDIATE, .srcOperand = false, .destOperand = true},
    [OP_JEQ] = {.opcode = OP_JEQ, .srcMode = AM_NONE, .destMode = AM_IMMEDIATE, .srcOperand = false, .destOperand = true},
    [OP_CMP] = {.opcode = OP_CMP, .srcMode = AM_IMMEDIATE | AM_REGISTER, .destMode = AM_IMMEDIATE | AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_RET] = {.opcode = OP_RET, .srcMode = AM_NONE, .destMode = AM_NONE, .srcOperand = false, .destOperand = false},
    [OP_RST] = {.opcode = OP_RST, .srcMode = AM_NONE, .destMode = AM_NONE, .srcOperand = false, .destOperand = false},
    [OP_HLT] = {.opcode = OP_HLT, .srcMode = AM_NONE, .destMode = AM_NONE, .srcOperand = false, .destOperand = false},
    [OP_LDR] = {.opcode = OP_LDR, .srcMode = AM_DIRECT, .destMode = AM_REGISTER, .srcOperand = true, .destOperand = true},
    [OP_STR] = {.opcode = OP_STR, .srcMode = AM_REGISTER, .destMode = AM_DIRECT, .srcOperand = true, .destOperand = true},

};

#endif // ISA_H