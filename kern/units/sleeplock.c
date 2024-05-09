/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sleeplock.h"

/**
 * @brief sleeplock initialization
 * 
 * @param slock sleeplock object
 */
void
sleeplock_init(sleeplock_t *slock) {
    if (slock == null)    panic("sleeplock_init(): null pointer");
    spinlock_init(&slock->guard_);
    slock->islock_ = 0;
}
