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

/**
 * @brief watching if kernel panic error happen over
 */
static inline void
assert(bool condition) {
    if (condition)    __asm__ __volatile__ ("hlt");
}

static inline void
load_gdtr(void *gdt48) {
    __asm__ ("lgdt %0" : :"m"(gdt48));
}

#endif
