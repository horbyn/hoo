/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_ASSERT_H__
#define __KERN_ASSERT_H__

/**
 * @brief watching if kernel panic error happen over
 */
#define ASSERT(condition)                               \
    do {                                                \
        if (condition)    __asm__ __volatile__ ("hlt"); \
    } while(0)

#endif
