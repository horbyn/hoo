/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "spinlock.h"

/**
 * @brief spinlock initialization
 * 
 * @param spin spinlock
 */
void
spinlock_init(spinlock_t *spin) {
    spin->islock_ = 0;                                      // nobody holds the lock
}

/**
 * @brief hold the spinlock
 * 
 * @param spin spinlock
 */
void
wait(spinlock_t *spin) {

    __asm__ ("1:\n\t"
             "testl $1, %0\n\t"
             "jnz 1b\n\t"
             "lock btsl $0, %0\n\t"
             "jc 1b"
             : "=m"(*spin) :: "cc");
}

/**
 * @brief release the lock
 * 
 * @param spin spinlock
 */
void
signal(spinlock_t *spin) {

    __asm__ ("movl $0, %0" :: "m"(*spin) :);
}