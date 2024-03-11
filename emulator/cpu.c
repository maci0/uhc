#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "isa.h"
#include "cpu.h"
#include "bus.h"

// Global variables
typedef uint64_t (*InstructionHandler)(Instruction instruction);
static InstructionHandler instructionHandlers[256]; //

static uint64_t registers[64];        // General Purpose Registers
static uint8_t ir[INSTRUCTION_WIDTH]; // Instruction Register - hold the current instruction
static uint64_t pc;                   // program counter
static uint64_t sp;                   // stack pointer - stack starts at end of 8MB memory minus 1MB, stack grows down
static uint64_t ra;                   // return address register, also known as link register
uint8_t itr = 0;                      // interrupt register
static uint64_t fp;                   // frame pointer

//  Status register
static struct flags
{
    uint8_t reserved : 4; // unused
    uint8_t overflow : 1; // overflow flag
    uint8_t carry : 1;    // Carry flag
    uint8_t sign : 1;     // sign flag
    uint8_t zero : 1;     // zero flag
} sr = {.zero = 0, .sign = 0, .carry = 0, .overflow = 0};

Instruction instruction;

extern uint8_t filebuf[1024];

// local functions signatures
static uint64_t nop(Instruction instruction);
static uint64_t mov(Instruction instruction);
static uint64_t push(Instruction instruction);
static uint64_t pop(Instruction instruction);
static uint64_t add(Instruction instruction);
static uint64_t sub(Instruction instruction);
static uint64_t mul(Instruction instruction);
static uint64_t _div(Instruction instruction);
static uint64_t jmp(Instruction instruction);
static uint64_t cmp(Instruction instruction);
static uint64_t jeq(Instruction instruction);
static uint64_t call(Instruction instruction);
static uint64_t ret(Instruction instruction);
static uint64_t rst(Instruction instruction);
static uint64_t hlt(Instruction instruction);

static void CPU_Reset();
static void CPU_Halt();
static void CPU_PushStack(uint64_t value);
static uint64_t CPU_PopStack();



static void CPU_ValidateInstruction();

void CPU_PushStack(uint64_t value) {
    print_debug("\n");
    BUS_Write(sp, value);
    sp -= 8;
}

uint64_t CPU_PopStack() {
    print_debug("\n");
    sp += 8;
    return BUS_Read(sp);
}

uint64_t CPU_GetValue(uint8_t addressing_mode, uint64_t operand)
{
    print_debug("addressing_mode: %u | operand: %lu\n", addressing_mode, operand);

    switch (addressing_mode)
    {
    case AM_IMMEDIATE:
        print_debug("return operand: %lu\n", operand);
        return operand;
        break;
    case AM_REGISTER:
        if (operand == 0)
            return 0; // r0 is always 0
        else
            print_debug("return operand: %lu\n", registers[operand]);
        return registers[operand];
        break;
    default:
        print_error("Invalid Addressing mode for Operand\n");
        exit(EXIT_FAILURE);
        break;
    }
    return 0;
}

uint64_t CPU_SetValue(uint8_t addressing_mode, uint64_t operand, uint64_t value)
{
    print_debug("addressing_mode: %u | operand: %lu | value: %lu\n", addressing_mode, operand, value);

    switch (addressing_mode)
    {
    case AM_IMMEDIATE:
        print_error("Invalid Addressing mode for Operand\n");
        exit(EXIT_FAILURE);
        break;
    case AM_REGISTER:
        if (operand == 0)
            break; // r0 is always 0 and can not be set to something else -- this can be used to discard items from the stack
        registers[operand] = value;
        break;
    default:
        print_error("Invalid Addressing mode for Operand\n");
        exit(EXIT_FAILURE);
        break;
    }

    return value;
}

static uint64_t nop(Instruction instruction)
{
    print_debug("\n");
    return 0;
}

static uint64_t mov(Instruction instruction)
{
    print_debug("\n");
    uint64_t value = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t push(Instruction instruction)
{
    print_debug("\n");
    uint64_t data = CPU_GetValue(instruction.destMode, instruction.destOperand);
    CPU_PushStack(data);

    return data;
}

static uint64_t pop(Instruction instruction)
{
    print_debug("\n");
    uint64_t data = CPU_PopStack();
    CPU_SetValue(instruction.destMode, instruction.destOperand, data);
    return data;
}

static uint64_t add(Instruction instruction)
{
    print_debug("\n");

    uint64_t v1 = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    uint64_t v2 = CPU_GetValue(instruction.destMode, instruction.destOperand);
    uint64_t value = v2 + v1;

    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t sub(Instruction instruction)
{
    print_debug("\n");

    uint64_t v1 = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    uint64_t v2 = CPU_GetValue(instruction.destMode, instruction.destOperand);
    uint64_t value = v2 - v1;

    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t mul(Instruction instruction)
{
    print_debug("\n");

    uint64_t v1 = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    uint64_t v2 = CPU_GetValue(instruction.destMode, instruction.destOperand);
    uint64_t value = v2 * v1;

    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t _div(Instruction instruction)
{
    print_debug("\n");

    uint64_t v1 = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    uint64_t v2 = CPU_GetValue(instruction.destMode, instruction.destOperand);
    uint64_t value = v1 / v2;

    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t jmp(Instruction instruction)
{
    print_debug("\n");
    pc = CPU_GetValue(instruction.destMode, instruction.destOperand);
    return pc;
}

static uint64_t cmp(Instruction instruction)
{
    print_debug("\n");
    sr.zero = 0;
    uint64_t v1 = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    uint64_t v2 = CPU_GetValue(instruction.destMode, instruction.destOperand);
    if (v1 == v2)
        sr.zero = 1;
    return sr.zero;
}

static uint64_t jeq(Instruction instruction)
{
    print_debug("\n");
    if (sr.zero == true)
    {
        pc = CPU_GetValue(instruction.destMode, instruction.destOperand);
    }
    return pc;
}

static uint64_t call(Instruction instruction)
{
    print_debug("\n");

    ra = pc;
    CPU_PushStack(ra); // Push Return Address to Stack

    pc = CPU_GetValue(instruction.destMode, instruction.destOperand);

    return pc;
}

static uint64_t ret(Instruction instruction)
{
    print_debug("\n");

    ra = CPU_PopStack(); // Pop Return Address from Stack

    pc = ra;
    ra = 0;

    return pc;
}

static uint64_t ldr(Instruction instruction)
{
    print_debug("\n");
    uint64_t value = BUS_Read(instruction.srcOperand);
    CPU_SetValue(instruction.destMode, instruction.destOperand, value);
    return value;
}

static uint64_t str(Instruction instruction)
{
    print_debug("\n");
    uint64_t value = CPU_GetValue(instruction.srcMode, instruction.srcOperand);
    BUS_Write(instruction.destOperand, value);
    return value;
}


static uint64_t rst(Instruction instruction)
{
    print_debug("\n");
    CPU_Reset();
    return 0;
}

static uint64_t hlt(Instruction instruction)
{
    print_debug("\n");
    CPU_Halt();
    return 0;
}

void CPU_FetchInstruction()
{
    memcpy(ir, &filebuf[INSTRUCTION_WIDTH * pc], INSTRUCTION_WIDTH);
    print_debug("%u %u %u %lu %lu\n", ir[0], ir[1], ir[2], *(uint64_t *)(ir + 3), *(uint64_t *)(ir + 11));
}

static void CPU_ValidateInstruction()
{
    print_debug("%u %u %u %lu %lu\n", instruction.opcode, instruction.srcMode, instruction.destMode, instruction.srcOperand, instruction.destOperand);

    if (instruction.opcode == 0)
    {
        print_debug("Instruction with 0 opcode \n");
        exit(EXIT_FAILURE);
    }

    if (instructionSet[instruction.opcode].opcode != instruction.opcode)
    {
        print_debug("Instruction not found in InstructionSet \n");
        exit(EXIT_FAILURE);
    }

    if ((instructionSet[instruction.opcode].srcMode & instruction.srcMode) != instruction.srcMode)
    {
        print_debug("Illegal Source mode \n");
        exit(EXIT_FAILURE);
    }

    if ((instructionSet[instruction.opcode].destMode & instruction.destMode) != instruction.destMode)
    {
        print_debug("Illegal Destination mode \n");
        exit(EXIT_FAILURE);
    }

    // Check source operand requirement
    if (instructionSet[instruction.opcode].srcOperand && instruction.srcOperand < 0)
    {
        print_debug("Source operand required but missing\n");
        exit(EXIT_FAILURE);
    }

    // Check destination operand requirement
    if (instructionSet[instruction.opcode].destOperand && instruction.destOperand < 0)
    {
        print_debug("Destination operand required but missing\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void CPU_DecodeInstruction()
{
    instruction.opcode = ir[0];
    instruction.srcMode = ir[1];
    instruction.destMode = ir[2];
    instruction.srcOperand = *(uint64_t *)(ir + 3);
    instruction.destOperand = *(uint64_t *)(ir + 11);
    print_debug("%u %u %u %lu %lu\n", instruction.opcode, instruction.srcMode, instruction.destMode, instruction.srcOperand, instruction.destOperand);

    CPU_ValidateInstruction();
}

uint64_t CPU_ExecuteInstruction()
{
    if (itr == 1) {print_info("INTERRUPT"); exit (EXIT_SUCCESS);}
    pc++;
    uint8_t index = instruction.opcode;

    print_debug("%u %u %u %lu %lu\n", instruction.opcode, instruction.srcMode, instruction.destMode, instruction.srcOperand, instruction.destOperand);

    InstructionHandler handler = instructionHandlers[index];
    if (handler)
    {
        return handler(instruction);
    }
    else
    {
        print_error("Unhandled instruction\n");
        exit(EXIT_FAILURE);
    }
}

void CPU_PrintRegisters()
{
    printf("PC: %lu | SP: %lu | FP: %lu | RA: %lu | R[0-10]: %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu | %lu\n",
           pc, sp, fp, ra,
           registers[0], registers[1], registers[2], registers[3], registers[4],
           registers[5], registers[6], registers[7], registers[8], registers[9], registers[10]);

    // printf("SR: %u | IR: %u %u %u %lu %lu", sr, ir[0], ir[1], ir[2], *(uint64_t *)(ir + 3), *(uint64_t *)(ir + 11));
    printf("SR: ");
    printf("%u", sr.reserved);
    printf("%u", sr.overflow);
    printf("%u", sr.carry);
    printf("%u", sr.sign);
    printf("%u", sr.zero);

    printf(" | IR: %u %u %u %lu %lu\n", ir[0], ir[1], ir[2], *(uint64_t *)(ir + 3), *(uint64_t *)(ir + 11));
}


void CPU_Halt()
{
    print_debug("\n");
    exit(EXIT_SUCCESS);
}

void CPU_Pause(){
    return;
}

void CPU_Reset()
{
    print_debug("\n");

    sr.zero = 0;
    sr.carry = 0;
    sr.sign = 0;
    sr.overflow = 0;

    sp = 8000; // start of the stack pointer
    fp = 0;
    ra = 0;
    pc = 0;
    itr = 0;
    memset(registers, 0, sizeof(registers));
    // memset(stack, 0, sizeof(stack));
    // memset(mem, 0, sizeof(mem)); // CPU can only access memory through the bus
    memset(ir, 0, sizeof(ir));
}

void CPU_Init()
{

    instructionHandlers[OP_NOP] = &nop;
    instructionHandlers[OP_MOV] = &mov;
    instructionHandlers[OP_PUSH] = &push;
    instructionHandlers[OP_POP] = &pop;
    instructionHandlers[OP_ADD] = &add;
    instructionHandlers[OP_SUB] = &sub;
    instructionHandlers[OP_MUL] = &mul;
    instructionHandlers[OP_DIV] = &_div;
    instructionHandlers[OP_CMP] = &cmp;
    instructionHandlers[OP_JMP] = &jmp;
    instructionHandlers[OP_JEQ] = &jeq;
    instructionHandlers[OP_CALL] = &call;
    instructionHandlers[OP_RET] = &ret;
    instructionHandlers[OP_RST] = &rst;

    instructionHandlers[OP_HLT] = &hlt;
    instructionHandlers[OP_LDR] = &ldr;
    instructionHandlers[OP_STR] = &str;

    CPU_Reset();
}




/*void print_state()
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
}*/