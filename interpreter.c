#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "isa.h"

#define BINFILE "test.bin"

// typedef struct Instruction Instruction;

void fetch_instruction(char *mem);
int decode_instruction(Instruction *instruction);
int execute_instruction(Instruction instruction);

uint64_t get_value(uint8_t mode, uint64_t operand);
int set_value(uint8_t mode, uint64_t operand, uint64_t value);

int reset_state();
void halt();

void print_state();

// Global variables
uint64_t registers[64];
uint8_t stack[4096]; // currently unused
uint8_t mem[8192];
uint8_t rom[4096];             // currently holds the current program as read from the binary file
uint64_t sp = 8000;               // Stack Pointer stack starts at mem[8000] and grows down
uint64_t pc = 0;               // Program Counter
uint64_t ra = 0;               // return address register
uint8_t sr = 0;                // Status Register
uint8_t ir[INSTRUCTION_WIDTH]; // instruction register

bool running = true;

/*
uint8_t *memory_access (uint64_t index) {
    if (index < 4096) {
        return &rom[index];
    } else if (index < (4096 + 8192)) {
        return &mem[index - 4096];
    } else if (index < (4096 + 8192 + 4096)) {
        return &stack[index - (4096 + 8192)];
    } else {
        printf("ERROR: memory access out of bounds");
        exit(EXIT_FAILURE);
        return NULL;
    }
}
*/

uint8_t *memory_access(uint64_t index)
{
    if (index < 8000)
    {
        return &mem[index];
    }
    else if (index >= 8000)
    {
        // reserve for MMIO
        // return MMIO[index];
        printf("ERROR: memory access out of bounds");
        return NULL;
    }
    else
    {
        return NULL;
    }
}

uint64_t get_value(uint8_t mode, uint64_t operand)
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
        // printf("get value: %lu\n", *(uint64_t *)(mem + operand));
        return *(uint64_t *)memory_access(operand);
        // return (uint64_t)mem[operand];
    case INDIRECT:
        // Handle INDIRECT mode or leave as unimplemented
        fprintf(stderr, "INDIRECT mode not implemented\n");
        exit(EXIT_FAILURE);
    default:
        fprintf(stderr, "Illegal mode\n");
        exit(EXIT_FAILURE);
    }
}

int set_value(uint8_t mode, uint64_t operand, uint64_t value)
{
    assert((mode & ALL_RW) == mode); // Ensure mode is within valid range for writing

    switch (mode)
    {
    case IMMEDIATE:
        fprintf(stderr, "Illegal mode for set_value\n");
        exit(EXIT_FAILURE);
    case REGISTER:
        registers[operand] = value;
        return 0;
    case DIRECT:
        // TODO: for now we only store 64bit values
        // printf("set value %lu\n", value);
        *(uint64_t *)memory_access(operand) = value;
        return 0;
    case INDIRECT:
        // IMMEDIATE is read-only, INDIRECT might be unimplemented
        fprintf(stderr, "Illegal or unimplemented mode for set_value\n");
        exit(EXIT_FAILURE);
    default:
        fprintf(stderr, "Illegal mode for set_value\n");
        exit(EXIT_FAILURE);
    }
}

void fetch_instruction(char *mem)
{
    memcpy(ir, &rom[INSTRUCTION_WIDTH * pc], INSTRUCTION_WIDTH);
}

int decode_instruction(Instruction *instruction)
{
    instruction->opcode = ir[0];
    instruction->addressing_mode1 = ir[1];
    instruction->addressing_mode2 = ir[2];
    instruction->operand1 = *(uint64_t *)(ir + 3);
    instruction->operand2 = *(uint64_t *)(ir + 11);

    printf("Instruction: %u %u %u %lu %lu\n", instruction->opcode, instruction->addressing_mode1, instruction->addressing_mode2, instruction->operand1, instruction->operand2);
    return 0;
}

int execute_instruction(Instruction instruction)
{
    pc++;
    // printf("Instruction: %u %u %u %lu %lu\n", instruction.opcode, instruction.addressing_mode1, instruction.addressing_mode2, instruction.operand1, instruction.operand2);

    switch (instruction.opcode)
    {
    case NOP:
        break;
    case MOV:
        set_value(instruction.addressing_mode2, instruction.operand2, get_value(instruction.addressing_mode1, instruction.operand1));
        break;
    case PUSH:
        sp -= 8;
        set_value(DIRECT, sp, get_value(instruction.addressing_mode1, instruction.operand1));
        break;
    case POP:
        set_value(instruction.addressing_mode1, instruction.operand1, get_value(DIRECT, sp));
        sp += 8;
        break;
    case ADD:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) + get_value(instruction.addressing_mode1, instruction.operand1)));
        break;
    case SUB:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) - get_value(instruction.addressing_mode1, instruction.operand1)));
        break;
    case MUL:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) * get_value(instruction.addressing_mode1, instruction.operand1)));
        break;
    case DIV:
        set_value(instruction.addressing_mode2, instruction.operand2, (get_value(instruction.addressing_mode2, instruction.operand2) / get_value(instruction.addressing_mode1, instruction.operand1)));
        break;
    case JMP:
        pc = get_value(instruction.addressing_mode1, instruction.operand1);
        break;
    case CALL:
        if (ra != 0)
        {
            printf("currently only call depth of 1 allowed\n");
            exit(EXIT_FAILURE);
        }
        ra = pc + 1;
        pc = get_value(instruction.addressing_mode1, instruction.operand1);
        break;
    case RET:
        // printf("RET CALLED: PC: %lu, RA: %lu\n", pc, ra);
        pc = ra;
        ra = 0;
        break;
    case CMP:
        if (get_value(instruction.addressing_mode1, instruction.operand1) == get_value(instruction.addressing_mode2, instruction.operand2))
        {
            // printf("equal\n");
            sr |= 0x1;
        }
        else
        {
            // printf("not equal\n");
            sr &= ~0x1;
        }
        break;
    case JEQ:
        if (sr & 0x1)
        {
            printf("attempting jump to %lu\n", instruction.operand1);
            pc = get_value(instruction.addressing_mode1, instruction.operand1);
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
        printf("%s: Invalid opcode: %d \n", __func__, instruction.opcode);
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
    memset(ir, 0, sizeof(ir));

    pc = 0;
    sp = 8000;
    sr = 0;
    ra = 0;
    running = true;
    return 0;
}
void halt()
{
    printf("Halting\n");
    running = false;
    // exit(EXIT_SUCCESS);
}

void init()
{
    FILE *binfile;
    binfile = fopen(BINFILE, "rb");
    assert(binfile != NULL && "Failed to open file");

    fseek(binfile, 0, SEEK_END);
    size_t filesize = ftell(binfile);
    rewind(binfile);

    size_t read = fread(rom, 1, filesize, binfile);
    if (read != filesize)
    {
        if (feof(binfile))
        {
            fputs("Reading error: unexpected end of file", stderr);
        }
        else if (ferror(binfile))
        {
            perror("Reading error");
        }
        exit(3);
    }
    fclose(binfile);
}

int main()
{
    Instruction instruction;

    reset_state();
    init();

    while (running == true)
    {
        fetch_instruction((char *)rom);
        decode_instruction(&instruction);
        execute_instruction(instruction);
        print_state();
    }
    print_state();
    return 0;
}

void print_state()
{
    printf("PC: %lu | SP: %lu | RA: %lu | R[0-10]: %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu\n",
           pc, sp, ra,
           registers[0], registers[1], registers[2], registers[3], registers[4],
           registers[5], registers[6], registers[7], registers[8], registers[9], registers[10]);

    // Print the first 10 elements of mem[]
    printf("SR: %u | IR: %u %u %u %lu %lu", sr, ir[0], ir[1], ir[2], *(uint64_t *)(ir + 3), *(uint64_t *)(ir + 11));
    printf(" | M[0-15]: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%02lX ", (uint64_t)mem[i]);
    }
    printf("\n");
}
