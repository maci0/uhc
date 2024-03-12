#include <stdint.h> 
#include <stdio.h>
/*
#include "../common/common.h"
#include "interrupts.h"



typedef uint64_t (*InterruptHandler)(Interrupt interrupt);
static InterruptHandler interruptHandlers[8];

static uint64_t _INT_None(Interrupt interrupt);
static uint64_t _INT_NMI(Interrupt interrupt);
static uint64_t _INT_IRQ(Interrupt interrupt);
static uint64_t _INT_CON(Interrupt interrupt);

void INT_Init(){
    print_debug("Initializing interrupts\n");

    interruptHandlers[INT_NONE] = &_INT_None;
    interruptHandlers[INT_NMI] = &_INT_NMI;
    interruptHandlers[INT_IRQ] = &_INT_IRQ;
    interruptHandlers[INT_CON] = &_INT_CON;
    return;
}

void INT_Tick(){
    print_debug("Checking interrupts\n");
    return;
}

void INT_SetInterrupt(Interrupt interrupt){
    print_debug("Setting interrupt\n");
    return;
}*/