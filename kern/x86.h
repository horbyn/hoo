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

/* This header may be included by assembly which cannot recognize C codes,
   so the following functions MUST define to macros */

/**
 * @brief enable intrrupt
 */
#define ENABLE_INTR()   __asm__ ("sti")

/**
 * @brief disenable intrrupt
 */
#define DISABLE_INTR()  __asm__ ("cli")

/**
 * @brief halt the machine
 */
#define HLT() ({ \
    DISABLE_INTR(); \
    __asm__ ("hlt"); \
})

/**
 * @brief fetch data from the specified port
 * 
 * @param port the specified port
 * @return data
 */
#define INB(port) ({ \
    uint8_t __val; \
    __asm__ volatile ("inb %w1, %b0" : "=a"(__val) : "d"(port)); \
    __val; \
})

/**
 * @brief data write to the specified port
 * 
 * @param val 8-bit data to write
 * @param port the specified port
 */
#define OUTB(val, port) ({ \
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "d"(port)); \
})

/**
 * @brief data flow reads from the specified port
 * 
 * @param flow_ptr data flow to write
 * @param len      size
 * @param port     the specified port
 */
#define INSW(flow_ptr, len, port) ({ \
    __asm__ volatile ("cld; rep insw" :: \
        "=D"(flow_ptr) : "c"(len), "d"(port)); \
})

/**
 * @brief data flow writes to the specified port
 * 
 * @param flow data flow to write
 * @param len  size
 * @param port the specified port
 */
#define OUTSW(flow_ptr, len, port) ({ \
    __asm__ volatile ("cld; rep outst" :: \
        "S"(flow_ptr), "c"(len), "d"(port)); \
})

#endif
