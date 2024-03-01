#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "isa.h"

#define BINFILE "test.bin"

typedef struct Instruction Instruction;

size_t fetch_instruction(FILE *file, uint8_t buffer[INSTRUCTION_WIDTH]);
int decode_instruction(uint8_t buffer[INSTRUCTION_WIDTH], Instruction *instruction);
int execute_instruction(Instruction instruction);

uint64_t get_byte(uint8_t mode, uint64_t operand);
int set_byte(uint8_t mode, uint64_t operand, uint64_t value);

int reset_state();
void halt();

void print_state();

// Global variables
uint64_t registers[64];
uint8_t stack[1024];
uint8_t mem[1024];
uint8_t sp = 0; // Stack Pointer
uint8_t pc = 0; // Program Counter
uint8_t sr = 0; // Status Register
bool running = true;

uint64_t get_byte(uint8_t mode, uint64_t operand)
{
    assert((mode & ALL) == mode); // Ensure mode is within valid range

    switch (mode)
    {
    case IMMEDIATE:
        return operand;
    case REGISTER:
        return registers[operand];
    case DIRECT:
    // TODO: for now we only load 64bit from memory
        //printf("get value: %lu\n", *(uint64_t *)(mem + operand));
        return *(uint64_t *)(mem + operand);
        //return (uint64_t)mem[operand];
    case INDIRECT:
        // Handle INDIRECT mode or leave as unimplemented
        fprintf(stderr, "INDIRECT mode not implemented\n");
        exit(EXIT_FAILURE);
    default:
        fprintf(stderr, "Illegal mode\n");
        exit(EXIT_FAILURE);
    }
}

int set_byte(uint8_t mode, uint64_t operand, uint64_t value)
{
    assert((mode & ALL_RW) == mode); // Ensure mode is within valid range for writing

    switch (mode)
    {
    case IMMEDIATE:
        fprintf(stderr, "Illegal mode for set_byte\n");
        exit(EXIT_FAILURE);
    case REGISTER:
        registers[operand] = value;
        return 0;
    case DIRECT:
    // TODO: for now we only store 64bit values
        //printf("set value %lu\n", value);
        *(uint64_t *)(mem + operand) = value;
        return 0;
    case INDIRECT:
        // IMMEDIATE is read-only, INDIRECT might be unimplemented
        fprintf(stderr, "Illegal or unimplemented mode for set_byte\n");
        exit(EXIT_FAILURE);
    default:
        fprintf(stderr, "Illegal mode for set_byte\n");
        exit(EXIT_FAILURE);
    }
}

size_t fetch_instruction(FILE *file, uint8_t buffer[INSTRUCTION_WIDTH])
{

    fseek(file, INSTRUCTION_WIDTH, pc * INSTRUCTION_WIDTH);
    //printf("File position indicator: %ld\n", ftell(file));

    size_t read = fread(buffer, sizeof(uint8_t), INSTRUCTION_WIDTH, file);
    if (read != INSTRUCTION_WIDTH)
    {
        perror("Failed to read instruction");
        exit(EXIT_FAILURE);
    }

    return read;
}

int decode_instruction(uint8_t buffer[INSTRUCTION_WIDTH], Instruction *instruction)
{
    instruction->opcode = buffer[0];
    instruction->addressing_mode1 = buffer[1];
    instruction->addressing_mode2 = buffer[2];
    instruction->operand1 = *(uint64_t *)(buffer + 3);
    instruction->operand2 = *(uint64_t *)(buffer + 11);

    printf("Instruction: %u %u %u %lu %lu\n", instruction->opcode, instruction->addressing_mode1, instruction->addressing_mode2, instruction->operand1, instruction->operand2);
    return 0;
}

int execute_instruction(Instruction instruction)
{
    //printf("Instruction: %u %u %u %lu %lu\n", instruction.opcode, instruction.addressing_mode1, instruction.addressing_mode2, instruction.operand1, instruction.operand2);

    switch (instruction.opcode)
    {
    case NOP:
        break;
    case MOV:
        set_byte(instruction.addressing_mode2, instruction.operand2, get_byte(instruction.addressing_mode1, instruction.operand1));
        break;
    case ADD:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) + get_byte(instruction.addressing_mode1, instruction.operand1)));
        break;
    case SUB:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) - get_byte(instruction.addressing_mode1, instruction.operand1)));
        break;
    case MUL:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) * get_byte(instruction.addressing_mode1, instruction.operand1)));
        break;
    case DIV:
        set_byte(instruction.addressing_mode2, instruction.operand2, (get_byte(instruction.addressing_mode2, instruction.operand2) / get_byte(instruction.addressing_mode1, instruction.operand1)));
        break;
    case JMP:
        pc = get_byte(instruction.addressing_mode1, instruction.operand1);
        break;
    case CMP:
        if (get_byte(instruction.addressing_mode1, instruction.operand1) == get_byte(instruction.addressing_mode2, instruction.operand2))
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
            pc = get_byte(instruction.addressing_mode1, instruction.operand1);
            sr &= ~0x1;
        }
        break;
    case RST:
        reset_state();
        break;
    case HLT:
        halt();
        break;
    default:
        printf("Invalid opcode");
        exit(EXIT_FAILURE);
        break;
    }

    return 0;
}

int reset_state()
{
    memset(registers, 0, sizeof(registers));
    memset(stack, 0, sizeof(stack));
    memset(mem, 0, sizeof(mem));

    pc = 0;
    sp = 0;
    sr = 0;
    running = true;
    return 0;
}
void halt()
{
    printf("Halting\n");
    running = false;
    //exit(EXIT_SUCCESS);
}

int main()
{
    Instruction instruction;                 // The instruction cache
    uint8_t buffer[INSTRUCTION_WIDTH]; // Read buffer for instructions
    FILE *binfile;                     // file pointer to binary file

    reset_state();

    binfile = fopen(BINFILE, "rb");
    assert(binfile != NULL && "Failed to open file");

    while (running == true)
    {
        pc++;
        fetch_instruction(binfile, buffer);
        decode_instruction(buffer, &instruction);
        execute_instruction(instruction);
        print_state();
    }

    fclose(binfile);
    return 0;
}

void print_state()
{
    printf("PC: %u | SP: %u | R[0-9]: %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu\n",
           pc, sp,
           registers[0], registers[1], registers[2], registers[3], registers[4],
           registers[5], registers[6], registers[7], registers[8], registers[9]);

    // Print the first 10 elements of mem[]
    printf("SR: %u | ##### |",sr);
    printf(" M[0-15]: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", (uint8_t)mem[i]);
    }
    printf("\n");
    
}
