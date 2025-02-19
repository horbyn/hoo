#pragma once
#ifndef __KERN_UTILITIES_SPINLOCK_H__
#define __KERN_UTILITIES_SPINLOCK_H__

#include "user/types.h"

/**
 * @brief spinlock 定义
 */
typedef struct spinlock {
    // 如果有人持有锁则非零
    uint32_t islock_;
} spinlock_t;

/**
 * @brief 测试是否有人占用着锁
 * 
 * @param spin spinlock
 * @retval 1: 被人占用
 * @retval 0: 没人占用
 */
static inline int
test(spinlock_t *spin) {
    int ret = 0;

    __asm__ ("\n\t"
        "testl $1, %1\n\t"
        "jnz 1f\n\t"
        "movl $0, %0\n\t"
        "jmp 2f\n\t"
        "1: movl $1, %0\n\t"
        "2:"
        : "=r"(ret) : "m"(*spin) : "cc");

    return ret;
}

void spinlock_init(spinlock_t *spin);
void wait(spinlock_t *spin);
void signal(spinlock_t *spin);

#endif
