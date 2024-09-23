/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "io.h"

static spinlock_t __spinlock_cga;

/**
 * @brief displayer initialization
 */
void
io_init(void) {
    spinlock_init(&__spinlock_cga);
    clear_screen();
}

/**
 * @brief clear the screen
 */
void
clear_screen(void) {
    wait(&__spinlock_cga);
    cga_clear_screen();
    signal(&__spinlock_cga);
}

/**
 * @brief kernel print character
 * @param ch the character to be printed
 */
static void
kprint_char(char ch) {
    wait(&__spinlock_cga);
    cga_putc(ch);
    signal(&__spinlock_cga);
}

/**
 * @brief kernel print string
 * @param str the string to be printed
 */
void
kprint_str(const char *str) {
    wait(&__spinlock_cga);
    cga_putstr(str, strlen(str));
    signal(&__spinlock_cga);
}

/**
 * @brief kernel print decimal digit
 * @param dig the dig to be printed
 */
void
kprint_int(int dig) {
    wait(&__spinlock_cga);
    cga_putint(dig);
    signal(&__spinlock_cga);
}

/**
 * @brief kernel print hex digit
 * @param hex the dig to be printed
 */
void
kprint_hex(uint32_t hex) {
    wait(&__spinlock_cga);
    cga_puthex(hex);
    signal(&__spinlock_cga);
}

/**
 * @brief formatting printing
 * 
 * @param fmt formatting string
 * @param ... variadic parameters
 */
void
kprintf(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            kprint_char(*fmt);
            continue;
        }

        // ignore if the last is '%'
        if (*(fmt + 1) == null)    return;
        else    ++fmt;

        // %% %c %s %d %x(%X) %p
        switch(*fmt) {
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
            kprint_char(*fmt);
            break;
        }
    }

    va_end(va);
}

/**
 * @brief print the stack frame so far
 */
static void
trace() {
    kprintf("========== Stack trace ==========\n");

    uint32_t ebp = 0, ret = 0;
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));
    uint32_t border = PGUP(ebp, PGSIZE);

    for (uint32_t i = 0; ebp <= border; ++i) {
        ret = *(uint32_t *)(ebp + sizeof(uint32_t));

        for (uint32_t space = 0; space < i; ++space) {
            kprintf("  ");
        }
        kprintf("- 0x%x", ret);

        // check whether it is interrupt stack
        if (ret == (uint32_t)isr_part3) {
            kprintf(" *");
            istackcpu_t *istack = 
                (istackcpu_t *)(ebp + sizeof(uint32_t) * 2 + sizeof(istackos_t));
            kprintf(" <- 0x%x", istack->oldeip_);
        }

        kprintf("\n");
        ebp = *(uint32_t *)(ebp);
    }
}

/**
 * @brief display some messages in collapse
 * 
 * @param extra extra message
 */
void
panic(const char *extra) {
    clear_screen();
    if (extra)
        kprintf("%s\n\n", extra);
    trace();
    disable_intr();
    hlt();
}
