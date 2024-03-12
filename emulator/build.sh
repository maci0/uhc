gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -g -o tisc-emu src/core/*.c src/memory/*.c src/devices/*.c src/main.c
#../assembler/tasm.py -o test.bin asm/test.asm > /dev/null
#gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -O3 -o tisc-emu cpu.c video.c bus.c rom.c clock.c main.c

#gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -g -o tisc-emu-gui cpu.c video.c bus.c rom.c gui.c -lSDL2 -lSDL2_ttf
