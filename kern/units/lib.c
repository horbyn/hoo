/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "lib.h"

/**
 * @brief calculate the strings size
 * 
 * @param str the string to be calculated
 * @return size 
 */
uint32_t
strlen(const char *str) {
    uint32_t len = 0;
    while (*str++ != 0)
        len++;
    return len;
}

/**
 * @brief setup the `fill` to the `buff` up to `bufflen` size
 * 
 * @param buff    the buff to be reseted
 * @param fill    the context in the buff returned
 * @param bufflen size
 */
void
memset(void *buff, uint8_t fill, uint32_t bufflen) {
    if (buff == null || bufflen == 0)    return;

    uint8_t *ptr = (uint8_t *)buff;
    while (bufflen--)
        *ptr++ = fill;
}

/**
 * @brief fill the buffer with null
 * 
 * @param buff    the buff to be reseted
 * @param fill    the context in the buff returned
 * @param bufflen size
 */
void
bzero(void *buff, uint32_t bufflen) {
    memset(buff, 0, bufflen);
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
memmove(void *dst, const void *src, uint32_t size) {
    if (dst == null)    return -1;
    if (src == null || src == dst)    return 0;

    uint8_t *pdst = (uint8_t *)dst + size - 1;
    const uint8_t *psrc = (uint8_t *)src + size - 1;
    while (size--)    *pdst-- = *psrc--;

    return 0;
}

/**
 * @brief compare two buffer
 * 
 * @param a buffer a
 * @param b buffer b
 * @param max_cmp_size the max size to be compared
 * @return true same
 * @return false not same
 */
bool
memcmp(const void *a, const void *b, uint32_t max_cmp_size) {
    const uint8_t *pa = a, *pb = b;

    uint32_t i = 0;
    for (; i < max_cmp_size; i++) {
        if (pa[i] != pb[i])    break;
    }

    return i == max_cmp_size ? true : false;
}
