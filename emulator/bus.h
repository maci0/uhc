#ifndef BUS_H
#define BUS_H

#include <stdint.h>

// Memory Memory Map
#define MEMORY_START 0x00000000 
#define MEMORY_END 0x00FFFFFF // 16 MB RAM

#define ROM_START 0x01000000 
#define ROM_END 0x010FFFFF // 1 MB ROM

#define MMIO_START 0x01100000  
#define MMIO_END 0x0110FFFF // 64 KB MMIO

uint64_t BUS_Read(uint64_t address);
uint64_t BUS_Write(uint64_t buf, uint64_t address);
uint64_t BUS_SendInterrupt(uint8_t interrupt);

void MMIO_Writer();


extern uint8_t itr; // Interrupt Register


#endif // BUS_H