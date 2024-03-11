#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../core/bus.h"
#include "../common/common.h"

static char buffer[1024];

// console control register
static struct flags
{
    uint8_t reserved : 3;
    uint8_t ENABLED : 1;
    uint8_t PARITY_ERR : 1;
    uint8_t OVERRUN : 1;
    uint8_t RXRDY : 1;
    uint8_t TXRDY : 1;
} ccr = {.TXRDY = 0, .RXRDY = 0, .OVERRUN = 0, .PARITY_ERR = 0, .ENABLED = 0};

// console data register
static uint8_t cdr[8];


const char *input_file = "/tmp/tisc64-in";
const char *output_file = "/tmp/tisc64-out";

//const char *tmp_output_file = "tisc64-fileout";
//static FILE *tmp_output_fd;

static int out_fd;
static int in_fd;

void CON_Init()
{

    ccr.TXRDY = 0;
    ccr.RXRDY = 0;
    ccr.OVERRUN = 0;
    ccr.PARITY_ERR = 0;
    ccr.ENABLED = 0;

    memset(buffer, 0, sizeof(buffer));
    memset(cdr, 0, sizeof(cdr));


    mkfifo(output_file, 0666);


    // Open the input pipe
    /* mkfifo(input_file, 0666); */

    /*
        tmp_output_fd = fopen(tmp_output_file, "a");
        if (tmp_output_fd == NULL)
        {
            perror("Failed to open file");
            fclose(tmp_output_fd);
            exit(EXIT_FAILURE);
        }*/

    ccr.ENABLED = 1;
}

void CON_Write(uint64_t address, uint64_t data)
{
    // If write address is 0, this is the ccr
    // If write address is 8, this is the cdr
    print_debug("address: %lu, data: %lu\n", address, data);
    if (address == 8)
        *((uint64_t *)&cdr[0]) = data;

    if (address == 0 && data == 1)
        ccr.TXRDY = 1;
}

uint64_t CON_Read(uint64_t address)
{
    print_debug("\n");
    if (address == 8)
        return *((uint64_t *)&cdr[0]);
    return 0;
}

void CON_Out()
{
    print_debug("\n");

    out_fd = open(output_file, O_WRONLY | O_SYNC | O_NONBLOCK);
    if (out_fd != -1)
    {
        //fprintf(tmp_output_fd, "Console stuff received %lu", cdr);
        write(out_fd, &cdr, sizeof(cdr));
        close(out_fd);
        ccr.TXRDY = false;
    }
}

void CON_In()
{
    print_debug("\n");

    printf("ready to receive stuff\n");

    // check if file /tmp/interrupt exists, if yes print "foo"
    int interrupt_file = open("/tmp/interrupt", O_RDONLY | O_SYNC | O_NONBLOCK);
    if (interrupt_file != -1)
    {
        printf("foo\n");
        BUS_SendInterrupt(1);
        close(interrupt_file);
    }


    /*in_fd = open(input_file, O_RDONLY | O_SYNC);
    if (in_fd == -1)
    {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }*/

    // BUS_SendInterrupt(); // send an interrupt so that an interrupt handler can read the character

}

void CON_Close()
{
    print_debug("\n");
    close(in_fd);
    close(out_fd);
}

void CON_Tick()
{
    print_debug("\n");
    if (ccr.RXRDY == true)
        CON_In();
    if (ccr.TXRDY == true)
        CON_Out();
}