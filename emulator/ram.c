#include "common.h"
#include "bus.h"
#include "rom.h"

static uint8_t ram[8388608]; // 8 Megabytes

uint64_t RAM_Read(uint64_t address)
{
    uint64_t data = *((uint64_t *)&ram[address]);
    return data;
}

uint64_t RAM_Write(uint64_t address, uint64_t data)
{
    *((uint64_t *)&ram[address]) = data;
    return data;
}