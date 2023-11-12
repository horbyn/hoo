/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_SPINLOCK_H__
#define __KERN_LIB_SPINLOCK_H__

#include "kern/x86.h"
#include "kern/types.h"

/**
 * @brief binary somaphore -- spinlock definition
 */
typedef struct spinlock {
    uint32_t islock_;                                       // true if someone holds the lock
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
        "testl $1, %0\n\t"
        "jnz 1f\n\t"
        "movl $0, %0\n\t"
        "jmp 2f\n\t"
        "1: movl $1, %0\n\t"
        "2:"
        : "+r"(ret) :: "cc");

    return ret;
}

void spinlock_init(spinlock_t *);
void wait(spinlock_t *);
void signal(spinlock_t *);

#endif
