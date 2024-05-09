/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "hoo.h"

/**
 * @brief get the page directory table of hoo thread
 * 
 * @return the page directory
 */
pgelem_t *
get_hoo_pgdir(void) {
    return (pgelem_t *)(SEG_PDTABLE * 16 + KERN_HIGH_MAPPING);
}

/**
 * @brief get the tss object of hoo thread
 * @note this tss object is only one
 * @return tss object
 */
tss_t *
get_hoo_tss(void) {
    static tss_t tss;
    return &tss;
}

/**
 * @brief get the pcb of hoo thread
 * 
 * @return the pcb
 */
pcb_t *
get_hoo_pcb(void) {
    return pcb_get(TID_HOO);
}

/**
 * @brief get the sleeplock of hoo thread
 * 
 * @return sleeplock
 */
sleeplock_t *
get_hoo_sleeplock(void) {
    // "hoo" need to sleep until "idle" completes initialization
    static sleeplock_t wait_idle_init;
    static bool is_init = false;
    if (!is_init) {
        sleeplock_init(&wait_idle_init);
        is_init = true;
    }
    return &wait_idle_init;
}
