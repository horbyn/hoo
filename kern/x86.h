#pragma once
#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#include "boot/config_boot.h"
#include "user/types.h"

extern uint8_t __kern_end[];

// ARDS 数量
#define ADDR_ARDS_NUM   \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS_CR))
// ARDS 结构
#define ADDR_ARDS_BASE  \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS))
#define ARDS_TYPE_OS        1
#define ARDS_TYPE_ARCH      2

#define BITS_PER_BYTE       8
#define INVALID_INDEX       (-1)
#define STACK_BOOT_TOP      ((SEG_PGTABLE)*16 + PGSIZE)
#define STACK_HOO_RING3     (STACK_BOOT_TOP + PGSIZE + KERN_HIGH_MAPPING)
#define STACK_HOO_RING0     (0x80000 + KERN_HIGH_MAPPING)
#define DIED_INSTRUCTION    (((uint32_t)(SEG_DIED))*16 + (OFF_DIED))

// 任务数量
#define MAX_TASKS_AMOUNT    1024

/**
 * @brief ARDS 结构体
 */
typedef struct {
    uint32_t base_low_;
    uint32_t base_hig_;
    uint32_t length_low_;
    uint32_t length_hig_;
    uint32_t type_;
} ards_t;

/**
 * @brief 开中断
 */
static inline void
enable_intr() {
    __asm__ ("sti");
}

/**
 * @brief 关中断
 */
static inline void
disable_intr() {
    __asm__ ("cli");
}

/**
 * @brief 停机
 */
static inline void
hlt() {
    __asm__ ("cli\n\thlt");
}

/**
 * @brief 从指定端口中读数据
 * 
 * @param port 指定一个端口
 * @return 数据
 */
static inline uint8_t
inb(uint16_t port) {
    uint8_t val;
    // %al --> val
    // port --> %dx
    // 相当于 `inb %dx, %al` 即数据从 %dx 到 %al
    __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "d"(port));
    return val;
}

/**
 * @brief 数据写入指定端口
 * 
 * @param val  要写入的 8 位数据
 * @param port 指定一个端口
 */
static inline void
outb(uint8_t val, uint16_t port) {
    // val --> %al
    // port --> %dx
    // 数据从 %al 传递到 %dx 指向的端口
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "d"(port));
}

/**
 * @brief 从特定端口读取数据流
 * 
 * @param flow_ptr 数据流缓冲区
 * @param len      缓冲区大小
 * @param port     指定一个端口
 */
static inline void
insw(void *flow, uint32_t len, uint16_t port) {
    __asm__ volatile ("cld; rep insw" ::
        "D"(flow), "c"(len), "d"(port));
}

/**
 * @brief 将数据流写入特定端口
 * 
 * @param flow 数据流缓冲区
 * @param len  缓冲区大小
 * @param port 指定一个端口
 */
static inline void
outsw(const void *flow, uint32_t len, uint16_t port) {
    __asm__ volatile ("cld; rep outsw" ::
        "S"(flow), "c"(len), "d"(port));
}

#endif
