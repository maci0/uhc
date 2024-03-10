#include <stdio.h>

#include "bus.h"
#include "rom.h"

static uint8_t rom[1048576]; // 1 Megabyte

uint64_t ROM_Read(uint64_t address){
    rom[0] = 69;

    //uint64_t value =  *((uint64_t *)&rom[address]);
    //return value;
    return rom[0]; // always return 69
}