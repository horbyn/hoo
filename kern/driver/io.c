/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "io.h"

/**
 * @brief i/o system initialization
 */
void
kinit_io(void) {
    cga_clear();
}


/**
 * @brief formatting printing
 * 
 * @param fmt formatting string
 * @param ... variadic parameters
 */
void
printf(const char *fmt, ...) {
    va_list va;
    VA_START(va, fmt);

    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            cga_putc(*fmt, DEF_IOATTR);
            continue;
        }

        // ignore if the last is '%'
        if (*(fmt + 1) == 0)    return;
        else    ++fmt;

        // %% %c %s %d %x(%X) %p
        switch(*fmt) {
        case '%': cga_putc('%', DEF_IOATTR); break;
        case 'c': VA_ARG(va, char); cga_putc(*((char *)va), DEF_IOATTR); break;
        case 's':
            VA_ARG(va, const char *);
            uint32_t addr = *((uint32_t *)va);
            cga_putstr((const char *)addr, DEF_IOATTR); break;
        case 'd': VA_ARG(va, int); cga_putdig(*((int *)va), 10, DEF_IOATTR); break;
        case 'p':
        case 'X':
        case 'x':
            VA_ARG(va, uint32_t);
            cga_putdig(*((uint32_t *)va), 16, DEF_IOATTR);
            break;
        default:
            // something unsupported displays directly
            // TODO: %f
            VA_ARG(va, POINTER_SIZE);
            cga_putc('%', DEF_IOATTR);
            cga_putc(*fmt, DEF_IOATTR);
            break;
        }
    }

    VA_END(va);
}

/**
 * @brief print the stack frame so far
 */
static void
trace() {
    printf("========== Stack trace ==========\n");

    uint32_t ebp = 0, ret = 0;
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));
    uint32_t border = PGUP(ebp, PGSIZE);

    for (uint32_t i = 0; ebp != 0 && ebp <= border; ++i) {
        ret = *(uint32_t *)(ebp + sizeof(uint32_t));

        for (uint32_t space = 0; space < i; ++space) {
            printf("  ");
        }
        printf("- 0x%x", ret);

        // check whether it is interrupt stack
        if (ret == (uint32_t)isr_part3) {
            printf(" *");
            istackcpu_t *istack = 
                (istackcpu_t *)(ebp + sizeof(uint32_t) * 2 + sizeof(istackos_t));
            printf(" <- 0x%x", istack->oldeip_);
        }

        printf("\n");
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
    cga_clear();
    if (extra != null)
        printf("%s\n\n", extra);
    trace();
    DISABLE_INTR();
    HLT();
}
