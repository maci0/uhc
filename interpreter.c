#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

int fetch_instruction(FILE *file);
int decode_instruction();
int execute_instruction(uint64_t source_value, uint64_t destination);

#define STACK_SIZE (1 << 20) // 1 Megabyte
#define REGISTER_COUNT (64)
#define INSTRUCTION_WIDTH (1 + 1 + 1 + 8 + 8)

typedef enum
{
    NONE = 0,
    IMMEDIATE = 1,
    REGISTER = 2,
    DIRECT = 4,
    INDIRECT = 8
} AddressingMode;

typedef enum
{
    NOP = 0x00,

    MOV = 0x01,

    ADD = 0x02,
    SUB = 0x03,
    MUL = 0x04,
    DIV = 0x05,

    AND = 0x06,
    OR = 0x07,
    XOR = 0x08,
    NOT = 0x09,
    LSH = 0x0A,
    RSH = 0x0B,

    JMP = 0x0C,
    CMP = 0x0D,
    JEQ = 0x0E,

    RST = 0xFE,
    HLT = 0xFF
} Opcode;

typedef struct
{
    uint8_t opcode;
    uint8_t addressing_mode1;
    uint8_t addressing_mode2;
    uint64_t operand1;
    uint64_t operand2;
} Instruction;

// Global variables
uint64_t registers[REGISTER_COUNT];
uint8_t stack[STACK_SIZE];
uint8_t sp = 0; // Stack Pointer
uint8_t pc = 0; // Program Counter
uint8_t sr = 0; // Status Register

Instruction cache;

int fetch_instruction(FILE *file)
{
    char buffer[INSTRUCTION_WIDTH];

    fseek(file, pc * INSTRUCTION_WIDTH, SEEK_SET);
    // printf("File position indicator: %ld\n", ftell(file));

    size_t read = fread(buffer, sizeof(char), INSTRUCTION_WIDTH, file);
    if (read != INSTRUCTION_WIDTH)
    {
        perror("Failed to read instruction");
        exit(EXIT_FAILURE);
    }

    cache.opcode = buffer[0];
    cache.addressing_mode1 = buffer[1];
    cache.addressing_mode2 = buffer[2];
    cache.operand1 = *(uint64_t *)(buffer + 3);
    cache.operand2 = *(uint64_t *)(buffer + 11);

    printf("Instruction: %u %u %u %lu %lu\n", cache.opcode, cache.addressing_mode1, cache.addressing_mode2, cache.operand1, cache.operand2);

    return 0;
}

// Function to decode instruction from binary data
int decode_instruction()
{
    pc++;

    uint64_t source_value, destination;

    switch (cache.addressing_mode1)
    {
    case NONE:
        break;
    case IMMEDIATE:
        source_value = cache.operand1;
        break;
    case REGISTER:
        source_value = registers[cache.operand1];
        break;
    case DIRECT:
        assert("Not implemented" && 0);
        break;
    case INDIRECT:
        assert("Not implemented" && 0);
        break;
    }

    switch (cache.addressing_mode2)
    {
    case NONE:
        break;
    case IMMEDIATE:
        assert("Invalid addressing mode for operand2" && 0);
        break;
    case REGISTER:
        destination = registers[cache.operand2];
        break;
    case DIRECT:
        assert("Not implemented" && 0);
        break;
    case INDIRECT:
        assert("Not implemented" && 0);
        break;
    }

    execute_instruction(source_value, destination);
    return 0;
}

// Function to execute an instruction
int execute_instruction(uint64_t source_value, uint64_t destination)
{
    switch (cache.opcode)
    {
    case NOP:
        break;
    case MOV:
        registers[cache.operand2] = source_value;
        break;
    case ADD:
        registers[cache.operand2] = source_value + destination;
        break;
    case SUB:
        registers[cache.operand2] = registers[cache.operand2] - source_value;
        break;
    case MUL:
        registers[cache.operand2] = source_value * destination;
        break;
    case DIV:
        registers[cache.operand2] = registers[cache.operand2] / source_value;
        break;
    case JMP:
        pc = cache.operand1;
        break;
    case CMP:
        if (source_value == destination)
        {
            sr |= 0x1;
        }
        else
        {
            sr &= ~0x1;
        }
        break;
    case JEQ:
        if (sr & 0x1)
        {
            pc = source_value;
            sr &= ~0x1;
        }
        break;
    case RST:
        memset(registers, 0, sizeof(registers));
        pc = 0;
        sp = 0;
        sr = 0;
        break;
    case HLT:
        exit(EXIT_SUCCESS);
        break;
    default:
        // Handle unknown opcode
        break;
    }

    return 0;
}

int main()
{
    // zero_registers
    memset(registers, 0, sizeof(registers));

    FILE *file = fopen("test.bin", "rb");
    if (!file)
    {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    while (file)
    {
        // print registers 0-9 on one line
        printf("PC: %u, SR: %u,  Registers 0-10: %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", pc, sr, registers[0], registers[1], registers[2], registers[3], registers[4], registers[5], registers[6], registers[7], registers[8], registers[9]);
        fetch_instruction(file);
        decode_instruction();
    }

    fclose(file);
    return 0;
}
