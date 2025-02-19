#pragma once
#ifndef __KERN_INTR_INTR_STACK_H__
#define __KERN_INTR_INTR_STACK_H__

#include "user/types.h"

/**
 * @brief OS 中断栈定义的寄存器组顺序对应于
 * `kern/intr/trampoline.S` 中存储环境上下文的顺序
 */
typedef struct interrupt_stack_os {
    uint32_t edi_;
    uint32_t esi_;
    uint32_t ebp_;
    uint32_t esp_;
    uint32_t ebx_;
    uint32_t edx_;
    uint32_t ecx_;
    uint32_t eax_;
    uint32_t gs_;
    uint32_t fs_;
    uint32_t es_;
    uint32_t ds_;
} __attribute__ ((packed)) istackos_t;

/**
 * @brief 处理器中断栈定义的寄存器组顺序对应于
 * 处理器执行 `iret` 时储存环境上下文的顺序
 */
typedef struct interrupt_stack_cpu {
    // unused when `iret`ing
    uint32_t vec_;
    // unused when `iret`ing
    uint32_t errcode_;
    void    *oldeip_;
    uint32_t oldcs_;
    uint32_t eflags_;

} __attribute__ ((packed)) istackcpu_t;

/**
 * @brief 线程栈
 */
typedef struct thread_stack {
    void *retaddr_;
} tstack_t;

#endif
