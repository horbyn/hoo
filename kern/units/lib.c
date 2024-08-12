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
 * @brief compare two strings
 * 
 * @param a string a
 * @param b string b
 * @retval true: same strings
 * @retval false: different strings
 */
bool
strcmp(const char *a, const char *b) {
    uint32_t alen = strlen(a), blen = strlen(b);
    if (alen != blen)     return false;
    for (uint32_t i = 0; i < alen; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
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
 * @brief seperate strings according specific character
 * 
 * @param str the string to be seperated
 * @param sep the character
 * @param cr  counts expected to meet the character (begin at 0)
 * (-1 means the last one)
 * @param result the result
 * @note e.g.
 * ("/dir1/dir2/dir3", '/', 0, {}) -> {""}
 * @note e.g.
 * ("/dir1/dir2/dir3", '/', 1, {}) -> {"dir1"}
 * @note e.g.
 * ("/dir1/dir2/dir3", '/', 2, {}) -> {"dir2"}
 * @note e.g.
 * ("/dir1/dir2/dir3", '/', 3, {}) -> {"dir3"}
 * @note e.g.
 * ("/dir1/dir2/dir3", '/', 4..., {}) -> {""}
 */
void
strsep(const char *str, char sep, int cr, char *result) {
    if (str == null || result == null)    return;

    uint32_t sz = strlen(str), j = 0;
    int count = 0;
    for (uint32_t i = 0; i < sz; ++i) {
        if (str[i] == sep) {
            result[j] = 0;
            if (count == cr)    return;
            else {
                ++count;
                j = 0;
            }
        } else    result[j++] = str[i];
    }
    if (cr != -1 && count < cr)    result[0] = 0;
    else    result[j] = 0;
}
