#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "common/common.h"
#include "core/cpu.h"
#include "core/bus.h"
#include "core/clock.h"
#include "devices/console.h"
#include "devices/fileout.h"
#include "devices/pty.h"


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
   // CON_Init();
    PTY_Init();

    while (!quit)
    {
        if (running)
        {
            CL_Tick(); // Clock tick
            CPU_Tick(); // CPU tick
            CPU_PrintRegisters(); // Print CPU registers
            //MMIO_Writer();
            //CON_Tick(); // Console tick
            //FO_Tick(); // Fileout device tick
            PTY_Tick(); // PTY Tick
            // BUS_SendInterrupt(1);
        }
    }

    return 0;
}