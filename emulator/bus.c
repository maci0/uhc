#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bus.h"

uint8_t mem[8192];
uint8_t rom[8192];


static bool is_in_range(uint64_t address, uint64_t start, uint64_t end)
{
    return address >= start && address <= end; 
}

// Determines the correct memory region for a given address and returns a pointer to it.
static uint64_t* BUS_Access(uint64_t address) {
    printf("bus access %lu\n", address);
    if (is_in_range(address, MEMORY_START, MEMORY_END)) {
        printf("bus access memory content %lu\n", (uint64_t)mem[(address - MEMORY_START)]);
        return (uint64_t*)&mem[(address - MEMORY_START)];
    } else if (is_in_range(address, ROM_START, ROM_END)) {
        // Adjust address offset based on ROM_START
        return (uint64_t*)&rom[(address - ROM_START)];
    } else {
        fprintf(stderr, "Error: %s: Address 0x%lx not in known address space.\n", __func__, address);
        exit(EXIT_FAILURE);
    }
}

uint64_t BUS_Read(uint64_t address)
{
    printf("%s: 0x%lx\n", __func__, address);
    uint64_t *location = BUS_Access(address);
    return *location;
}

uint64_t BUS_Write(uint64_t address, uint64_t data)
{
    printf("%s: 0x%lx, %lu\n",__func__, address, data);
    uint64_t *location = BUS_Access(address);
    *location = data;
    return 0;
}