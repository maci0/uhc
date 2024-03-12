#ifndef RAM_H
#define RAM_H

#include <stdint.h>
extern uint8_t ram[8388608];

uint64_t RAM_Read(uint64_t address);
uint64_t RAM_Write(uint64_t address, uint64_t data);

#endif // RAM_H