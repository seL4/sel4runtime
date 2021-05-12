/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <sel4runtime/stddef.h>
#include <sel4runtime/stdint.h>

#define ALIGN_UP(x, n) (((x) + (n) - 1) & ~((n) - 1))
#define ALIGN_DOWN(x, n) ((x) & ~((n) - 1))

#define ROUND_DOWN(n, b) \
    ({ typeof (n) _n = (n); \
       typeof (b) _b = (b); \
       _n - (_n % _b); \
    })

#define ROUND_UP(n, b) \
    ({ typeof (n) _n = (n); \
       typeof (b) _b = (b); \
       (_n + (_n % _b == 0 ? 0 : (_b - (_n % _b)))); \
    })

/*
 * We have our own copies of memset and memcpy to avoid conflicts and
 * dependencies on libc.
 */

void *__sel4runtime_memcpy(
    void *restrict dest,
    const void *restrict src,
    sel4runtime_size_t n
);
void *__sel4runtime_memset(void *dest, int c, sel4runtime_size_t n);
