/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "disp.h"

static spinlock_t __slock_cga;

/**
 * @brief cga hardware initialization
 */
void
init_cga(void) {
    spinlock_init(&__slock_cga);
    clear_screen();
}

/**
 * @brief kernel print character
 * @param ch the character to be printed
 */
void
kprint_char(char ch) {
    wait(&__slock_cga);
    cga_putc(ch);
    signal(&__slock_cga);
}

/**
 * @brief kernel print string
 * @param str the string to be printed
 */
void
kprint_str(const char *str) {
    wait(&__slock_cga);
    cga_putstr(str, strlen(str));
    signal(&__slock_cga);
}

/**
 * @brief kernel print decimal digit
 * @param dig the dig to be printed
 */
void
kprint_int(int dig) {
    wait(&__slock_cga);
    cga_putint(dig);
    signal(&__slock_cga);
}

/**
 * @brief kernel print hex digit
 * @param hex the dig to be printed
 */
void
kprint_hex(uint32_t hex) {
    wait(&__slock_cga);
    cga_puthex(hex);
    signal(&__slock_cga);
}

/**
 * @brief kernel formatting print
 * @param format formatting string
 * @param ... other
 */
void
kprintf(const char *format, ...) {
    va_list va;
    va_start(va, format);

    for (; *format; ++format) {
        if (*format != '%') {
            kprint_char(*format);
            continue;
        }

        // ignore if the last is '%'
        if (*(format + 1) == null)    return;
        else    ++format;

        // %% %c %s %d %x(%X) %p
        switch(*format) {
        case '%': kprint_char('%'); break;
        case 'c': va_arg(va, char); kprint_char(*((char *)va)); break;
        case 's':
            va_arg(va, const char *);
            uint32_t addr = *((uint32_t *)va);   // get address
            kprint_str((const char *)addr); break;
        case 'd': va_arg(va, int); kprint_int(*((int *)va)); break;
        case 'p':
        case 'X':
        case 'x': va_arg(va, uint32_t); kprint_hex(*((uint32_t *)va)); break;
        default:
            // something unsupported displays directly
            // TODO: %f
            va_arg(va, POINTER_SIZE);
            kprint_char('%');
            kprint_char(*format);
            break;
        }
    }

    va_end(va);
}
