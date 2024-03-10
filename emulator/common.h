#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>


#define print_debug(...)                                                   \
    do                                                                     \
    {                                                                      \
        if (debug)                                                         \
        {                                                                  \
            fprintf(stderr, "%s:%d:%s(): ", __FILE__, __LINE__, __func__); \
            fprintf(stderr, __VA_ARGS__);                                  \
        }                                                                  \
    } while (0)

#define print_error(...)                                               \
    do                                                                 \
    {                                                                  \
        fprintf(stderr, "%s:%d:%s(): ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__);                                  \
    } while (0)

#define print_info(...)                                                \
    do                                                                 \
    {                                                                  \
        fprintf(stderr, "%s:%d:%s(): ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__);                                  \
    } while (0)
    
#endif // COMMON_H