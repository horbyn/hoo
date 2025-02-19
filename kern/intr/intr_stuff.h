#pragma once
#ifndef __KERN_INTR_INTR_STUFF_H__
#define __KERN_INTR_INTR_STUFF_H__

#include "user/types.h"

#define IDT_ENTRIES_NUM     256

typedef void (*isr_t)(void);

/**
 * @brief IDT 段描述符
 */
typedef struct {
    /*
     * 63           48 47 46 45 44 43     40 39    32
     * ┌──────────────┼──┼─────┼──┼─────────┼────────┐
     * │Offset 31..16 │P │ DPL │ 0│Gate Type│Reserved│
     * └──────────────┴──┴─────┴──┴─────────┴────────┘
     * 31               16 15                       0
     * ┌──────────────────┼──────────────────────────┐
     * │Segment Selector  │  Offset 15..0            │
     * └──────────────────┴──────────────────────────┘
     */

    // 中断发生时用来加载 EIP
    uint16_t    isr_low_;
    // 中断发生时用来加载 CS
    uint16_t    selector_;
    uint8_t     reserved_;
    uint8_t     attributes_;
    // 中断发生时用来加载 EIP
    uint16_t    isr_high_;
} __attribute__((packed)) idt_t;

/**
 * @brief IDTR 定义
 */
typedef struct {
    uint16_t	limit_;
    uint32_t	base_;
} __attribute__((packed)) idtr_t;

/**
 * @brief 特权级枚举
 */
typedef enum privilege_level {
    PL_KERN = 0,
    PL_USER = 3
} privilege_t;

/**
 * @brief 门描述符枚举
 */
typedef enum gate_descriptor {
    INTER_GATE = 0x0e,
    TRAP_GATE = 0x0f
} gatedesc_t;

#endif
