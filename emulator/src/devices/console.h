#ifndef CONSOLE_H
#define CONSOLE_H

void CON_Init();
void CON_Close();
void CON_Tick();
uint64_t CON_Read(uint64_t address);
void CON_Write(uint64_t address, uint64_t data);


#endif // CONSOLE_H