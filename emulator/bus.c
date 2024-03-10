#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bus.h"
#include "common.h"

uint8_t mem[8388608];
uint8_t rom[1048576];
uint8_t mmio[65536];

static bool debug = true;

extern uint8_t itr;

// simulate a device that can write into a file
// # MMIO_Writer output register: 0x01100008, enable register: 0x01100000
void MMIO_Writer(){
    //print_debug("current MMIO state: mmio[0]: %u, mmio[1]: %u\n ", mmio[0], mmio[8]);
    if (mmio[0] == 1){
        print_debug("mmio[8] was written to: %u\n", mmio[8]);

        FILE *file;

        file = fopen("MMIO_Writer.txt", "ab");

        if (file == NULL) {
            printf("Error opening file!\n");
            return;
        }
        fprintf(file, "%c", mmio[8]);
        fflush(file);

        fclose(file);
    }
    return;
}

static bool is_in_range(uint64_t address, uint64_t start, uint64_t end)
{
    return address >= start && address <= end;
}

// Determines the correct memory region for a given address and returns a pointer to it.
static uint64_t *BUS_Access(uint64_t address)
{
    print_debug("bus access %lu\n", address);
    if (is_in_range(address, MEMORY_START, MEMORY_END))
    {
        print_debug("bus access memory content %lu\n", (uint64_t)mem[(address - MEMORY_START)]);
        return (uint64_t *)&mem[(address - MEMORY_START)];
    }
    else if (is_in_range(address, ROM_START, ROM_END))
    {
        // Adjust address offset based on ROM_START
        return (uint64_t *)&rom[(address - ROM_START)];
    }
    else if (is_in_range(address, MMIO_START, MMIO_END))
    {
        print_debug("bus access MMIO %lu\n", (uint64_t)mmio[(address - MMIO_START)]);
        return (uint64_t *)&mmio[(address - MMIO_START)];
    }
    else
    {
        print_error("Error: %s: Address 0x%lx not in known address space.\n", __func__, address);
        exit(EXIT_FAILURE);
    }
}

uint64_t BUS_Read(uint64_t address)
{
    print_debug("accessing address 0x%lx\n", address);
    uint64_t *location = BUS_Access(address);
    return *location;
}

uint64_t BUS_Write(uint64_t address, uint64_t data)
{
    printf("ADDRESS: %lu\n",address);
    print_debug("writing data to address 0x%lx, %lu\n", address, data);
    uint64_t *location = BUS_Access(address);
    *location = data;
    return 0;
}

uint64_t BUS_SendInterrupt(uint8_t interrupt)
{
    itr = interrupt;
    return 0;
}
