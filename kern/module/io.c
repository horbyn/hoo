/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "io.h"
#include "kern/utilities/format.h"
#include "kern/driver/cga/cga.h"

/**
 * @brief i/o system initialization
 */
void
kinit_io(void) {
    cga_clear();
    cga_init();
    cga_set_attribute(SCBUFF_COLOR_WHITE, SCBUFF_STYLE_LIGHT);
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
    VA_START(va, fmt);
    format(fmt, va, (void *)FD_STDOUT);
    VA_END(va);
}
