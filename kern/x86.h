/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#include "types.h"
#include "conf/Config.h"

// Eflags

#define EFLAGS_CP   0x1                                     // carry flag: carry if 1
#define EFLAGS_PF   0x4                                     // parity flag: parity even if 1
#define EFLAGS_AF   0x10                                    // auxiliary carry flag: auxiliary carry if 1
#define EFLAGS_ZF   0x40                                    // zero flag: zero if 1
#define EFLAGS_SF   0x80                                    // sign flag: negative if 1
#define EFLAGS_TF   0x100                                   // trap flag
#define EFLAGS_IF   0x200                                   // interrupt enable flag: enable interrupt if 1
#define EFLAGS_DF   0x400                                   // direction flag: down if 1
#define EFLAGS_OF   0x800                                   // overflow flag: overflow if 1

/**
 * @brief enum of privilege level
 */
typedef enum privilege_level {
    PL_KERN = 0,
    PL_USER = 3
} privilege_t;

/**
 * @brief enum of gate descriptor
 */
typedef enum gate_descriptor {
    INTER_GATE = 0x0e,
    TRAP_GATE = 0x0f
} gatedesc_t;

/**
 * @brief enable intrrupt
 */
static inline void
enable_intr() {
    __asm__ ("sti");
}

/**
 * @brief disenable intrrupt
 */
static inline void
disable_intr() {
    __asm__ ("cli");
}

/**
 * @brief fetch data from the specified port
 * 
 * @param port the specified port
 * @return data
 */
static inline uint8_t
inb(uint16_t port) {
    uint8_t val;
    // %al --> val
    // port --> %dx
    // is equal of `inb %dx, %al` that
    // the data is from %dx to %al
    __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "d"(port));
    return val;
}

/**
 * @brief data write to the specified port
 * 
 * @param val 8-bit data to write
 * @param port the specified port
 */
static inline void
outb(uint8_t val, uint16_t port) {
    // val --> %al
    // port --> %dx
    // data from %al is transported to port %dx pointed to
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "d"(port));
}

/**
 * @brief data flow reads from the specified port
 * 
 * @param flow data flow to write
 * @param len  size
 * @param port the specified port
 */
static inline void
insl(void *flow, size_t len, uint16_t port) {
    __asm__ volatile ("cld; rep insl" :
        "=D"(flow), "=c"(len) :
        "0"(flow), "1"(len), "d"(port) :
        "memory", "cc");
}

/**
 * @brief data flow writes to the specified port
 * 
 * @param flow data flow to write
 * @param len  size
 * @param port the specified port
 */
static inline void
outsl(const void *flow, size_t len, uint16_t port) {
    __asm__ volatile ("cld; rep outsl" :
        "=S"(flow), "=c"(len) :
        "0"(flow), "1"(len), "d"(port) :
        "cc");
}

#endif
