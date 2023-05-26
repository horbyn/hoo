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
 * @brief fetch data from the specified port
 * 
 * @param port the specified port
 * @return data
 */
static inline uint32_t
inl(uint16_t port) {
    uint32_t val = 0;
    __asm__ volatile ("inl %w1, %k0" : "=a"(val) : "d"(port));
    return val;
}

/**
 * @brief data write to the specified port
 * 
 * @param val 32-bit data to write
 * @param port the specified port
 */
static inline void
outl(uint32_t val, uint16_t port) {
    __asm__ volatile ("outl %k0, %w1" : : "a"(val), "d"(port));
}

#endif
