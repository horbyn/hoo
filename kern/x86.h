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

/**
 * @brief lgdt instruction
 * @param gdt48 gdtr format
 */
static inline void
load_gdtr(void *gdt48) {
    __asm__ ("lgdt (%0)" : :"r"(gdt48));
}

/**
 * @brief enable paging
 * @param pd_addr page dir table addr
 */
static inline void
paging(void *pd_addr) {
    __asm__ ("movl %0, %%cr3" : :"r"(pd_addr));
    __asm__ ("\r\n"
        "movl %cr0,       %eax\r\n"
        "orl $0x80000000, %eax\r\n"
        "movl %eax,       %cr0");
}

#endif
