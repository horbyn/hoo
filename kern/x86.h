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
#include "boot/config_boot.h"

extern uint8_t __kern_end[];
// the null pointer is not zero but a special address 
extern void *null;

// the ARDS amount addr
#define ADDR_ARDS_NUM   \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS_CR))
// the ARDS itself addr
#define ADDR_ARDS_BASE  \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS))
#define ARDS_TYPE_OS        1
#define ARDS_TYPE_ARCH      2
#define BITS_PER_BYTE       8
#define INVALID_INDEX       (-1)
#define STACK_BOOT_TOP      ((SEG_PGTABLE)*16 + PGSIZE)
#define STACK_HOO_RING3     (STACK_BOOT_TOP + PGSIZE + KERN_HIGH_MAPPING)
#define STACK_HOO_RING0     (0x80000 + KERN_HIGH_MAPPING)

// the task maxinum amount
#define MAX_TASKS_AMOUNT    1024

/**
 * @brief ards structure
 */
typedef struct {
    uint32_t base_low_;
    uint32_t base_hig_;
    uint32_t length_low_;
    uint32_t length_hig_;
    uint32_t type_;
} ards_t;

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
    __asm__ ("cli\n\thlt");
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
 * @param flow_ptr data flow to write
 * @param len      size
 * @param port     the specified port
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
