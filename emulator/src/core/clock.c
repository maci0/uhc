#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


#include "clock.h"

static struct timespec last_tick_time;

void CL_Tick() {
    if (BENCHMARK == true) return;

    struct timespec current_time, sleep_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // Calculate the delta time since the last tick in nanoseconds
    uint64_t delta_ns = (current_time.tv_sec - last_tick_time.tv_sec) * 1000000000L + (current_time.tv_nsec - last_tick_time.tv_nsec);

    // Period of one cycle in nanoseconds
    uint64_t period_ns = 1000000000L / CLOCK_FREQUENCY;

    // Calculate the sleep time required to maintain the desired clock frequency
    if (delta_ns < period_ns) {
        sleep_time.tv_sec = (period_ns - delta_ns) / 1000000000L;
        sleep_time.tv_nsec = (period_ns - delta_ns) % 1000000000L;

        nanosleep(&sleep_time, NULL);
    }

    // Update the last tick time to the current time
    clock_gettime(CLOCK_MONOTONIC, &last_tick_time);
}