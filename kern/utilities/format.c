/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "format.h"
#include "kern/types.h"
#include "kern/fs/files.h"
#include "kern/hoo/hoo.h"
#include "kern/driver/cga/cga.h"
#include "kern/utilities/format.h"
#include "kern/utilities/cache_buff.h"
#include "user/lib.h"

/**
 * @brief output a character
 * @param ch       character
 * @param redirect output direction
 */
static void
outputc(char ch, void *redirect) {
    if (redirect == (void *)FD_STDOUT || redirect == (void *)FD_STDERR)
        files_write(FD_STDOUT, &ch, 1);
    else    cachebuff_write((cachebuff_t *)redirect, &ch, 1);
}

/**
 * @brief output a string
 * @param str      string
 * @param redirect output direction
 */
static void
outputstr(const char *str, void *redirect) {
    if (redirect == (void *)FD_STDOUT || redirect == (void *)FD_STDERR)
        files_write(FD_STDOUT, str, strlen(str));
    else    cachebuff_write((cachebuff_t *)redirect, str, strlen(str));
}

/**
 * @brief output digit
 * @param digit    digit
 * @param base     base
 * @param redirect output direction
 */
static void
outputdig(uint32_t digit, uint8_t base, void *redirect) {

    static char __arr_dig[32];
    int i = 0;

    do {
        int remaider = digit % base;
        if (remaider < 10)    __arr_dig[i] = (char)('0' + remaider);
        else    __arr_dig[i] = (char)('a' + (remaider - 10));
        digit /= base;
        ++i;
    } while (digit);

    while (i)    outputc(__arr_dig[--i], redirect);
}

/**
 * @brief formatting handler
 * 
 * @param fmt      formatting string
 * @param args     variable arguments
 * @param redirect output direction
 */
void
format(const char *fmt, va_list args, void *redirect) {

    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            outputc(*fmt, redirect);
            continue;
        }

        // ignore if the last is '%'
        if (*(fmt + 1) == 0)    return;
        else    ++fmt;

        // %% %c %s %d %x(%X) %p
        switch(*fmt) {
        case '%': outputc('%', redirect); break;
        case 'c': VA_ARG(args, char); outputc(*((char *)args), redirect); break;
        case 's':
            VA_ARG(args, const char *);
            uint32_t addr = *((uint32_t *)args);
            outputstr((const char *)addr, redirect);
            break;
        case 'd': VA_ARG(args, int); outputdig(*((int *)args), 10, redirect); break;
        case 'p':
        case 'X':
        case 'x':
            VA_ARG(args, uint32_t);
            outputdig(*((uint32_t *)args), 16, redirect);
            break;
        default:
            // something unsupported displays directly
            // TODO: %f
            VA_ARG(args, POINTER_SIZE);
            outputc('%', redirect);
            outputc(*fmt, redirect);
            break;
        }
    }

}
