#ifndef PTY_H
#define PTY_H

#include <stdint.h>

int PTY_Init();
void PTY_Tick();
uint64_t PTY_Read(uint64_t address);
void PTY_Write(uint64_t address, uint64_t data);
void PTY_Destroy();


#endif // PYT_H