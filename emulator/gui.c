#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"
#include "bus.h"
#include "video.h"

#define BINFILE "test.bin"
// #define CLOCK_FREQUENCY 1000000 // 1 MHz clock speed (example)
#define CLOCK_FREQUENCY 1 // 1 MHz clock speed (example)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"

// Global state
static bool running = true;
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

    TTF_Font *font = TTF_OpenFont(FONT_PATH, 40);
    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;
    TTF_Init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow(
        "hello_sdl2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 1;
    }
    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color textBackgroundColor = {0xFF, 0xFF, 0xFF, 0xFF};
    SDL_Texture *text = NULL;
    SDL_Rect textRect;

    SDL_Surface *textSurface = TTF_RenderText_Shaded(font, "Red square", textColor, textBackgroundColor);
    SDL_UpdateWindowSurface(window);

    while (!quit)
    {
        running = false;
        clock_t current_clock = clock();
        if (current_clock >= next_clock_pulse)
        {
            running = true;
            next_clock_pulse += CLOCKS_PER_SEC / CLOCK_FREQUENCY;
        }
        if (running)
        {
            CPU_FetchInstruction();
            CPU_ExecuteInstruction();
            CPU_PrintRegisters();
        }
    }

    return 0;
    SDL_DestroyWindow(window);
    SDL_Quit();
}