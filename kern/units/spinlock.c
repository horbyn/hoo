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
    if (spin == null)    panic("spinlock_init(): null pointer");
    // nobody holds the lock
    spin->islock_ = 0;
}

/**
 * @brief hold the spinlock
 * 
 * @param spin spinlock
 */
void
wait(spinlock_t *spin) {
    __asm__ ("1:");

    // return 1 if the spin was occupied
    while (test(spin));

    __asm__ ("\n\t"
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
