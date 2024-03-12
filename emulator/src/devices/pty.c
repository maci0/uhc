#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "pty.h"
#include "../common/common.h"
#include "../core/bus.h"

#define PTY_BUF_SIZE 256


extern uint8_t itr;

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

static int master_fd = 0;
static char *slave_name;
static char read_buf[PTY_BUF_SIZE];


void PTY_In();
void PTY_Out();

void PTY_Write(uint64_t address, uint64_t data)
{
    // If write address is 0, this is the ccr
    // If write address is 8, this is the start of the buffer
    print_debug("address: %lu, data: %lu\n", address, data);
    if (address == 8)
        *((uint64_t *)&cdr[0]) = data;

    if (address == 0 && data == 1)
        ccr.TXRDY = 1;
}

uint64_t PTY_Read(uint64_t address)
{
    print_debug("\n");
    if (address == 8)
        return *((uint64_t *)&cdr[0]);
    return 0;
}


int PTY_Init()
{
    ccr.TXRDY = 0;
    ccr.RXRDY = 0;
    ccr.OVERRUN = 0;
    ccr.PARITY_ERR = 0;
    ccr.ENABLED = 0;

    memset(cdr, 0, sizeof(cdr));
    memset (read_buf, 0, PTY_BUF_SIZE);

    print_debug("\n");
    // Open a pseudoterminal device
    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1)
    {
        perror("posix_openpt");
        return EXIT_FAILURE;
    }

    // Grant access to the slave pseudoterminal
    if (grantpt(master_fd) == -1)
    {
        perror("grantpt");
        close(master_fd);
        return EXIT_FAILURE;
    }

    // Unlock the slave pseudoterminal
    if (unlockpt(master_fd) == -1)
    {
        perror("unlockpt");
        close(master_fd);
        return EXIT_FAILURE;
    }

    // Get the pathname of the slave pseudoterminal
    slave_name = ptsname(master_fd);
    if (slave_name == NULL)
    {
        perror("ptsname");
        close(master_fd);
        return EXIT_FAILURE;
    }

    print_debug("Pseudoterminal: %s\n", slave_name);
    return 0;
}

void PTY_Tick()
{
    print_debug("\n");
    PTY_In();
   // PTY_Out();
}

void PTY_Out()
{
    print_debug("\n");

    //char write_buf[] = "Hello, PTY!\r\n";

   // ssize_t num_written = write(master_fd, write_buf, sizeof(write_buf));
    ssize_t num_written = write(master_fd, read_buf, sizeof(read_buf));
    if (num_written == -1)
    {
        perror("write to master pty");
        return;
    }
}

void PTY_In()
{
    print_debug("\n");

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(master_fd, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int ready = select(master_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ready == -1)
    {
        perror("select");
        return;
    }

    if (FD_ISSET(master_fd, &read_fds))
    {
        // Read from the master side of the PTY
        ssize_t num_read = read(master_fd, read_buf, PTY_BUF_SIZE - 1);
        if (num_read == -1)
        {
            perror("read from master pty");
            return;
        }

        // Null-terminate the string read and print it
        read_buf[num_read] = '\0';
        print_debug("Read from PTY: %s\n", read_buf);
        BUS_SendInterrupt(3);
        PTY_Out(); // simple echo

    }
}

void PTY_Destroy()
{
}