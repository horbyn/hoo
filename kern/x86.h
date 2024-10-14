/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#include "types.h"

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
 * @brief halt the machine
 */
static inline void
hlt() {
    disable_intr();
    __asm__ ("hlt");
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
insw(void *flow, uint32_t len, uint16_t port) {
    __asm__ volatile ("cld; rep insw" ::
        "D"(flow), "c"(len), "d"(port));
}

/**
 * @brief data flow writes to the specified port
 * 
 * @param flow data flow to write
 * @param len  size
 * @param port the specified port
 */
static inline void
outsw(const void *flow, uint32_t len, uint16_t port) {
    __asm__ volatile ("cld; rep outsw" ::
        "S"(flow), "c"(len), "d"(port));
}

#endif
