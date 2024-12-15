/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sleeplock.h"
#include "kern/panic.h"
#include "kern/sched/tasks.h"

/**
 * @brief sleeplock initialization
 * 
 * @param sleep sleeplock object
 */
void
sleeplock_init(sleeplock_t *sleep) {
    if (sleep == null)    panic("sleeplock_init(): null pointer");
    spinlock_init(&sleep->guard_);
    sleep->islock_ = 0;
}

/**
 * @brief sleeping to wait the resource
 * 
 * @param sl sleeplock
 */
void
wait_sleep(sleeplock_t *sl) {
    if (sl == null)    panic("wait_sleep(): null pointer");
    wait(&sl->guard_);
    while (sl->islock_)    sleep(sl, &sl->guard_);
    sl->islock_ = 1;
    signal(&sl->guard_);
}

/**
 * @brief release the sleeplock
 * 
 * @param sl sleeplock
 */
void
signal_sleep(sleeplock_t *sl) {
    if (sl == null)    panic("signal_sleep(): null pointer");
    wait(&sl->guard_);
    sl->islock_ = 0;
    wakeup(sl);
    signal(&sl->guard_);
}
