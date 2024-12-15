/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_SLEEPLOCK_H__
#define __KERN_UTILITIES_SLEEPLOCK_H__

#include "spinlock.h"

/**
 * @brief sleep lock
 */
typedef struct sleeplock {
    // make sure hold this sleeplock atomically, like the guard
    spinlock_t guard_;
    // true if someone holds the lock
    uint32_t islock_;
} sleeplock_t;

void sleeplock_init(sleeplock_t *slock);
void wait_sleep(sleeplock_t *sl);
void signal_sleep(sleeplock_t *sl);

#endif
