#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../core/bus.h"
#include "../memory/rom.h"
#include "../memory/ram.h"
#include "../common/common.h"
#include "../devices/console.h"

uint8_t mmio[65536];

extern uint8_t itr; // The CPUs interrupt register

typedef enum
{
    DEVICE_RAM,
    DEVICE_ROM,
    DEVICE_MMIO,
    DEVICE_CONSOLE,
    DEVICE_UNKNOWN // For error handling
} DeviceType;

// simulate a device that can write into a file
// # MMIO_Writer output register: 0x01100008, enable register: 0x01100000
void MMIO_Writer()
{
    // print_debug("current MMIO state: mmio[0]: %u, mmio[1]: %u\n ", mmio[0], mmio[8]);
    if (mmio[0] == 1)
    {
        print_debug("mmio[8] was written to: %u\n", mmio[8]);

        FILE *file;

        file = fopen("MMIO_Writer.txt", "ab");

        if (file == NULL)
        {
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

uint64_t BUS_Map(uint64_t address)
{
    if (is_in_range(address, RAM_START, RAM_END))
    {
        return DEVICE_RAM;
    }
    else if (is_in_range(address, ROM_START, ROM_END))
    {
        return DEVICE_ROM;
    }
    else if (is_in_range(address, MMIO_START, MMIO_END))
    {
        return DEVICE_MMIO;
    }
    else if (is_in_range(address, CONSOLE_START, CONSOLE_END))
    {
        return DEVICE_CONSOLE;
    }
    else
    {
        return DEVICE_UNKNOWN;
    }
}

uint64_t BUS_Read(uint64_t address)
{
    uint64_t device = BUS_Map(address);
    switch (device)
    {
    case DEVICE_RAM:
        return RAM_Read(address - RAM_START);
        break;
    case DEVICE_ROM:
        return ROM_Read(address - ROM_START);
        break;
    case DEVICE_MMIO:
        return *((uint64_t *)&mmio[address - MMIO_START]);
        break;
    case DEVICE_CONSOLE:
        return CON_Read((address - CONSOLE_START));
        break;
    default:
        print_error("Unsupported address: 0x%lx\n", address);
        exit(EXIT_FAILURE);
    }
}

uint64_t BUS_Write(uint64_t address, uint64_t data)
{
    uint64_t device = BUS_Map(address);
    switch (device)
    {
    case DEVICE_RAM:
        return RAM_Write(address - RAM_START, data);
        break;
    case DEVICE_ROM:
        exit(EXIT_FAILURE);
        break;
    case DEVICE_MMIO:
        *((uint64_t *)&mmio[address - MMIO_START]) = data;
        break;
    case DEVICE_CONSOLE:
        CON_Write((address - CONSOLE_START), data);
        break;
    default:
        print_error("Unsupported address: 0x%lx\n", address);
        exit(EXIT_FAILURE);
    }
    return data;
}

uint64_t BUS_SendInterrupt(uint8_t interrupt)
{
    itr = interrupt;
    return 0;
}
