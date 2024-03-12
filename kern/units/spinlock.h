/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UNITS_SPINLOCK_H__
#define __KERN_UNITS_SPINLOCK_H__

#include "kern/types.h"
#include "kern/x86.h"

/**
 * @brief binary somaphore -- spinlock definition
 */
typedef struct spinlock {
    // true if someone holds the lock
    uint32_t islock_;
} spinlock_t;

void spinlock_init(spinlock_t *spin);
void wait(spinlock_t *spin);
void signal(spinlock_t *spin);

#endif
