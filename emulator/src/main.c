#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "common/common.h"
#include "core/cpu.h"
#include "core/bus.h"
#include "devices/video.h"
#include "core/clock.h"

#define BINFILE "test.bin"

// Global state
static bool running = true;
static bool quit = false;
bool debug = true;

uint8_t filebuf[1024];

void loadfile()
{
    FILE *binfile;
    binfile = fopen(BINFILE, "rb");

    fseek(binfile, 0, SEEK_END);
    size_t filesize = ftell(binfile);
    rewind(binfile);

    size_t read = fread(filebuf, 1, filesize, binfile);
    if (read != filesize)
    {
        if (feof(binfile))
        {
            fputs("Reading error: unexpected end of file", stderr);
        }
        else if (ferror(binfile))
        {
            perror("Reading error");
        }
        exit(3);
    }
    fclose(binfile);
}

int main(int argc, char *args[])
{

    loadfile();
    CPU_Init();

    while (!quit)
    {
        if (running)
        {
            CL_Tick();
            CPU_FetchInstruction();
            CPU_DecodeInstruction();
            CPU_ExecuteInstruction();
            CPU_PrintRegisters();
            MMIO_Writer();
            // BUS_SendInterrupt(1);
        }
    }

    return 0;
}