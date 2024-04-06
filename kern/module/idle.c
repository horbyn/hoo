/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "idle.h"

pgelem_t __pgdir_idle[PGDIR_SIZE] __attribute__((aligned(4096)));
pcb_t *__pcb_idle;

/**
 * @brief get the tss object of idle thread
 * @note this tss object is only one
 * @return tss object
 */
tss_t *
get_idle_tss(void) {
    static tss_t __tss;
    return &__tss;
}
