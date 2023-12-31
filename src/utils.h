#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define List(T)         \
    struct              \
    {                   \
        T *ptr;         \
        int len, alloc; \
    }
#define Append(list, ...)                                                               \
    do                                                                                  \
    {                                                                                   \
        if ((list)->alloc == (list)->len)                                               \
        {                                                                               \
            if ((list)->alloc == 0)                                                     \
                (list)->alloc = 1;                                                      \
            (list)->alloc *= 2;                                                         \
            (list)->ptr = realloc((list)->ptr, sizeof((list)->ptr[0]) * (list)->alloc); \
            if (!(list)->ptr)                                                           \
            {                                                                           \
                fprintf(stderr, "out of memory\n");                                     \
                exit(1);                                                                \
            }                                                                           \
        }                                                                               \
        (list)->ptr[(list)->len++] = (__VA_ARGS__);                                     \
    } while (0)
#define End(list) (&(list).ptr[(list).len])
#define Pop(list) (list)->ptr[assert((list)->len > 0), --(list)->len]

#define sstrcpy(dest, src)                                                                   \
    do                                                                                        \
    {                                                                                         \
        static_assert(sizeof(src) > sizeof(char *), "src must be an array, not a pointer");   \
        static_assert(sizeof(dest) > sizeof(char *), "dest must be an array, not a pointer"); \
        static_assert(sizeof(dest) >= sizeof(src), "not enough room in dest");                \
        strcpy((dest), (src));                                                                \
    } while (0)

#endif
