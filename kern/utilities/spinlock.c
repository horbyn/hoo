#include "spinlock.h"
#include "kern/panic.h"
#include "kern/x86.h"

/**
 * @brief spinlock 初始化
 * 
 * @param spin spinlock
 */
void
spinlock_init(spinlock_t *spin) {
    if (spin == null)    panic("spinlock_init(): null pointer");
    // 没人持有锁
    spin->islock_ = 0;
}

/**
 * @brief 申请 spinlock
 * 
 * @param spin spinlock
 */
void
wait(spinlock_t *spin) {
    __asm__ ("1:");

    // 如果出现 spin 则返回 1
    while (test(spin));

    __asm__ ("\n\t"
        "lock btsl $0, %0\n\t"
        "jc 1b"
        : "=m"(*spin) :: "cc");
}

/**
 * @brief 释放锁
 * 
 * @param spin spinlock
 */
void
signal(spinlock_t *spin) {
    __asm__ ("movl $0, %0" :: "m"(*spin) :);
}
