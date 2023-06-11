#pragma once

#include <stddef.h>
#include <stdio.h>

#define nullCheck(p) {                                      \
    if (p == NULL) {                                        \
        fprintf(stderr, "Failed to allocate memory.\n");    \
        exit(2);                                            \
    }                                                       \
}
