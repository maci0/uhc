#include <stdio.h>
#include <stdint.h>

#include "../common/common.h"
#include "../core/bus.h"
#include "fileout.h"


// enable register: 0x01100000 | output register: 0x01100008, 
// mapped to        0          |                  8

static uint8_t registers[16];



void FO_Write(uint64_t address, uint64_t data)
{
    // If write address is 0, this is the ccr
    // If write address is 8, this is the cdr
    print_debug("address: %lu, data: %lu\n", address, data);
    if (address == 8)
    *((uint64_t *)&registers[8]) = data;

    // if the enable register is written to, set the enable bit
    if (address == 0 && data == 1)
        registers[0] = 1;
}

// device can not be read from
uint64_t FO_Read(uint64_t address)
{
    print_debug("\n");
    return 0;
}



// simulate a device that can write into a file
void FO_Tick()
{
    print_debug("\n");
    // print_debug("current MMIO state: mmio[0]: %u, mmio[1]: %u\n ", mmio[0], mmio[8]);
    if (registers[0] == 1)
    {
        print_debug("mmio[8] was written to: %u\n", registers[8]);

        FILE *file;

        file = fopen("fileout.txt", "ab");

        if (file == NULL)
        {
            printf("Error opening file!\n");
            return;
        }
        fprintf(file, "%c", registers[8]);
        fflush(file);

        fclose(file);
    }
    // disable the device
    registers[0] = 0;
    return;
}