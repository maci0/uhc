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

static int master_fd = 0;
static char *slave_name;

void PTY_In();
void PTY_Out();

int PTY_Init()
{
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
    //PTY_Out();
}

void PTY_Out()
{
    print_debug("\n");

    ssize_t num_written;
    char write_buf[] = "Hello, PTY!\r\n";

    num_written = write(master_fd, write_buf, sizeof(write_buf));
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
        char read_buf[PTY_BUF_SIZE];
        ssize_t num_read;

        // Read from the master side of the PTY
        num_read = read(master_fd, read_buf, PTY_BUF_SIZE - 1);
        if (num_read == -1)
        {
            perror("read from master pty");
            return;
        }

        // Null-terminate the string read and print it
        read_buf[num_read] = '\0';
        print_debug("Read from PTY: %s\n", read_buf);
        // BUS_SendInterrupt(1);
    }
}

void PTY_Destroy()
{
}