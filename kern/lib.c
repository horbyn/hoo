#include "lib.h"

uint32_t
strlen(const char *str) {
    uint32_t len = 0;
    while (*str++ != 0)
        len++;
    return len;
}
