/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UNITS_CIRCULAR_BUFFER_H__
#define __KERN_UNITS_CIRCULAR_BUFFER_H__

#include "spinlock.h"

/**
 * @brief buffer for caching characters
 * (MUST BE used after "tasks system" initialization)
 */
typedef struct circular_buffer {
    uint32_t    capacity_;
    char        *buff_;
    int         head_;
    int         tail_;
    spinlock_t slock_;
} cclbuff_t;

cclbuff_t *cclbuff_alloc(uint32_t capacity);
void       cclbuff_free(cclbuff_t *cclbuff);
bool       cclbuff_put(cclbuff_t *cclbuff, char c);
char       cclbuff_get(cclbuff_t *cclbuff);

#endif
