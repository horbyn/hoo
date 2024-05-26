/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#include "boot/config_boot.h"
#include "types.h"

#define MM_BASE             0x100000
#define PGSIZE              0x1000
#define STACK_BOOT_BOTTOM   ((SEG_PGTABLE)*16 + PGSIZE)
#define STACK_HOO_RING3     (STACK_BOOT_BOTTOM + PGSIZE + KERN_HIGH_MAPPING)
#define STACK_HOO_RING0     (0x80000 + KERN_HIGH_MAPPING)
#define TICKS_PER_SEC       1000

// the ARDS amount addr
#define ADDR_ARDS_NUM       (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS_CR))

// the ARDS itself addr
#define ADDR_ARDS_BASE      (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS))
#define DIED_INSTRUCTION    (((uint32_t)(SEG_DIED))*16 + (OFF_DIED))

// carry flag: carry if 1
#define EFLAGS_CP           0x1  
// parity flag: parity even if 1
#define EFLAGS_PF           0x4  
// auxiliary carry flag: auxiliary carry if 1
#define EFLAGS_AF           0x10 
// zero flag: zero if 1
#define EFLAGS_ZF           0x40 
// sign flag: negative if 1
#define EFLAGS_SF           0x80 
// trap flag
#define EFLAGS_TF           0x100
// interrupt enable flag: enable interrupt if 1
#define EFLAGS_IF           0x200
// direction flag: down if 1
#define EFLAGS_DF           0x400
// overflow flag: overflow if 1
#define EFLAGS_OF           0x800
// the task maxinum amount
#define MAX_TASKS_AMOUNT    1024

/**
 * @brief thread id
 */
typedef uint32_t            tid_t;

/**
 * @brief ards structure
 */
typedef struct {
    uint32_t base_low_;
    uint32_t base_hig_;
    uint32_t length_low_;
    uint32_t length_hig_;
    uint32_t type_;
} __attribute__((packed)) ards_t;

/**
 * @brief ards type
 */
typedef enum {
    // OS can use
    ards_type_os = 1,
    // arch reserves
    ards_type_arch
} ards_type_t;

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
