/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_SPINLOCK_H__
#define __KERN_UTILITIES_SPINLOCK_H__

#include "user/types.h"

/**
 * @brief binary somaphore -- spinlock definition
 */
typedef struct spinlock {
    // true if someone holds the lock
    uint32_t islock_;
} spinlock_t;

/**
 * @brief test whether the lock is used
 * 
 * @param spin lock
 * @retval 1: used
 * @retval 0: free
 */
static inline int
test(spinlock_t *spin) {
    int ret = 0;

    __asm__ ("\n\t"
        "testl $1, %1\n\t"
        "jnz 1f\n\t"
        "movl $0, %0\n\t"
        "jmp 2f\n\t"
        "1: movl $1, %0\n\t"
        "2:"
        : "=r"(ret) : "m"(*spin) : "cc");

    return ret;
}

void spinlock_init(spinlock_t *spin);
void wait(spinlock_t *spin);
void signal(spinlock_t *spin);

#endif
