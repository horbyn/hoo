/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "hoo.h"

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
