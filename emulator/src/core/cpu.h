#ifndef CPU_H
#define CPU_H

void CPU_Init();
void CPU_Tick();
void CPU_PrintRegisters();
void CPU_FetchInstruction();
void CPU_DecodeInstruction();
uint64_t CPU_ExecuteInstruction();

void CPU_CheckInterrupts();

extern uint8_t itr;

#endif // CPU_H