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
    bool islock_;                                           // true if someone holds the lock
} spinlock_t;

void spinlock_init(spinlock_t *);
void wait(spinlock_t *);
void signal(spinlock_t *);

#endif
