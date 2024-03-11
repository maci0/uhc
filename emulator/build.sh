gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -g -o tisc-emu cpu.c video.c bus.c rom.c ram.c main.c
../assembler/asm.py test.asm > /dev/null
#gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -O3 -o tisc-emu cpu.c video.c bus.c rom.c main.c

#gcc -std=c11 -pedantic-errors -Werror -Wall -pedantic -g -o tisc-emu-gui cpu.c video.c bus.c rom.c gui.c -lSDL2 -lSDL2_ttf