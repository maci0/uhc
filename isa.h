#ifndef ISA_H
#define ISA_H

#include <stdint.h>
#include <stdio.h>

typedef struct Instruction Instruction;
typedef void (*FunctionPointer)(Instruction*);
// Function prototypes
void encode(Instruction* self);
void decode(Instruction* self);

#define INSTRUCTION_WIDTH (1 + 1 + 1 + 8 + 8)

typedef enum {
    NONE = 0,
    IMMEDIATE = 1,
    REGISTER  = 2,
    DIRECT = 4,
    INDIRECT = 8,
    ALL = IMMEDIATE | REGISTER | DIRECT | INDIRECT,
    ALL_RW = REGISTER | DIRECT | INDIRECT,
} AddressingModes;

// Declare and initialize a global instance



typedef enum
{
    NOP = 0x00,

    MOV = 0x01,

    ADD = 0x02,
    SUB = 0x03,
    MUL = 0x04,
    DIV = 0x05,

    AND = 0x06,
    OR  = 0x07,
    XOR = 0x08,
    NOT = 0x09,
    LSH = 0x0A,
    RSH = 0x0B,

    JMP = 0x0C,
    CMP = 0x0D,
    JEQ = 0x0E,

    LD8  = 240,
    LD16 = 241,
    LD32 = 242,
    LD64 = 243,
    ST8  = 244,
    ST16 = 245,
    ST32 = 246,
    ST64 = 247,

    RST = 0xFE,
    HLT = 0xFF
} Opcode;

struct Instruction {
    uint8_t opcode;
    uint8_t addressing_mode1;
    uint8_t addressing_mode2;
    uint64_t operand1;
    uint64_t operand2;
    FunctionPointer encode;
    FunctionPointer decode;
};

// Function implementations
void encode(Instruction* self) {
    printf("Encode function called!\n");
}

void decode(Instruction* self) {
    printf("Decode function called!\n");
}

#endif // ISA_H