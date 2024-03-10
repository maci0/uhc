#ifndef BUS_H
#define BUS_H

#include <stdint.h>

// Memory regions
#define MEMORY_START 0x00000000
#define MEMORY_END 0x00FFFFFF // 16 MB RAM

#define ROM_START 0x80000000
#define ROM_END 0x80400000 // 4 MB ROM

#define MMIO_START 0xFFFF0000
#define MMIO_END 0xFFFFFFFF // 64 KB MMIO

uint64_t BUS_Read(uint64_t address);
uint64_t BUS_Write(uint64_t buf, uint64_t address);

#endif // BUS_H