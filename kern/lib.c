#include "lib.h"

uint32_t
strlen(const char *str) {
    uint32_t len = 0;
    while (*str++ != 0)
        len++;
    return len;
}

void
memset(void *buff, uint8_t fill, size_t bufflen) {
    if (buff == null || bufflen == 0)    return;

    uint8_t *ptr = (uint8_t *)buff;
    while (bufflen--)
        *ptr++ = fill;
}
