/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "spinlock.h"

/**
 * @brief spinlock initialization
 * 
 * @param spin spinlock
 */
void
spinlock_init(spinlock_t *spin) {
    spin->islock_ = false;                                  // nobody holds the lock
}

/**
 * @brief hold the spinlock
 * 
 * @param spin spinlock
 */
void
wait(spinlock_t *spin) {

    // wait if the lock is holded by someone
    while (spin->islock_ == true);

    disable_intr();

    // hold the lock
    spin->islock_ = true;

    enable_intr();
}

/**
 * @brief release the lock
 * 
 * @param spin spinlock
 */
void
signal(spinlock_t *spin) {
    disable_intr();

    spin->islock_ = false;

    enable_intr();
}
