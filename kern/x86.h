/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

/**
 * @brief enable intrrupt
 */
static inline void
enable_intr() {
    __asm__ ("cli");
}

/**
 * @brief disenable intrrupt
 */
static inline void
disable_intr() {
    __asm__ ("sti");
}

#endif
