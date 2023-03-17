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

/**
 * @brief copy memory
 * 
 * @param dst destination
 * @param src source
 * @param size size
 * @retval -1 error
 * @retval 0 success
 */
int
memmove(void *dst, const void *src, size_t size) {
    if (dst == null)    return -1;
    if (src == null || src == dst)    return 0;

    uint8_t *pdst = (uint8_t *)dst + size - 1;
    const uint8_t *psrc = (uint8_t *)src + size - 1;
    while (size--)    *pdst-- = *psrc--;

    return 0;
}
