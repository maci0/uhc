#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef enum
{
    INT_NONE,
    INT_NMI,
    INT_IRQ,
    INT_CON, // console interrupt

} Interrupt;

extern uint8_t itr; // interrupt register - in cpu.c


//void INT_Init();
//void INT_Tick();


#endif // INTERRUPTS_H