#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "isa.h"

void CPU_Init();
void CPU_PrintRegisters();
void CPU_FetchInstruction();
void CPU_DecodeInstruction();
uint64_t CPU_ExecuteInstruction();

#endif // CPU_H