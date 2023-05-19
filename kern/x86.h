/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#define MM_BASE             0x100000                        // memory will be traced
#define KERN_HIGH_MAPPING   0xc0000000
#define PGSIZE              4096
#define PGDOWN(x, align)    ((x) & ~(align - 1))
#define PGUP(x, align)      (PGDOWN((x + align - 1), align))

/**
 * @brief watching if kernel panic error happen over
 */
#define ASSERT(condition)                               \
    do {                                                \
        if (condition)    __asm__ __volatile__ ("hlt"); \
    } while(0)

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

static inline void
load_gdtr(void *gdt48) {
    __asm__ ("lgdt %0" : :"m"(gdt48));
}

#endif
