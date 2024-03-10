#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"
#include "bus.h"
#include "video.h"

#define BINFILE "test.bin"
//#define CLOCK_FREQUENCY 1000000 // 1 MHz clock speed (example)
#define CLOCK_FREQUENCY 1 // 1 Hz clock speed (example)

// Global state
static bool running = true;
static bool benchmark = true;
static bool quit = false;

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
    clock_t start_clock = clock();
    clock_t next_clock_pulse = start_clock + CLOCKS_PER_SEC / CLOCK_FREQUENCY;

    loadfile();
    CPU_Init();

    while (!quit)
    {
        if (!benchmark)
        {
            running = false;
            clock_t current_clock = clock();
            if (current_clock >= next_clock_pulse)
            {
                running = true;
                next_clock_pulse += CLOCKS_PER_SEC / CLOCK_FREQUENCY;
            }
        }
        if (running)
        {
            CPU_FetchInstruction();
            CPU_DecodeInstruction();
            CPU_ExecuteInstruction();
            CPU_PrintRegisters();
        }
    }

    return 0;
}