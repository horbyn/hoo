/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "lib.h"

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
