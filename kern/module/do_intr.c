#include "do_intr.h"
#include "kern/driver/8042/8042.h"
#include "kern/driver/ata/ata_irq.h"
#include "kern/intr/routine.h"
#include "kern/module/log.h"
#include "kern/syscall/syscall.h"

static idtr_t __idtr;
__attribute__((aligned(0x4))) isr_t __isr[IDT_ENTRIES_NUM];
__attribute__((aligned(0x8))) static idt_t __idt[IDT_ENTRIES_NUM];

/**
 * @brief 初始化 ISR 和 IDT
 */
void
kinit_isr_idt(void) {
    // 设置默认的 ISR 数组
    for (uint32_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_isr_entry(&__isr[i], (isr_t)isr_default);

    // 设置特定的 ISR
    set_isr_entry(&__isr[ISR14_PAGEFAULT], (isr_t)page_fault);
    set_isr_entry(&__isr[ISR32_TIMER], (isr_t)timer);
    set_isr_entry(&__isr[ISR33_KEYBOARD], (isr_t)ps2_intr);
    set_isr_entry(&__isr[ISR46_HARD1], (isr_t)ata_irq_intr);
    set_isr_entry(&__isr[ISR128_SYSCALL], (isr_t)syscall);

    // 设置 IDT 数组
    // 首先让所有 IDT 元素都指向默认的处理函数，然后 IDT #128 指向 DPL=3 的系统调用
    for (uint32_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(&__idt[i], PL_KERN, INTER_GATE, (uint32_t)isr_part1[i]);
    set_idt_entry(&__idt[ISR128_SYSCALL], PL_USER, TRAP_GATE,
        (uint32_t)isr_part1[ISR128_SYSCALL]);

    // 加载 IDTR
    __idtr.limit_ = sizeof(__idt) - 1;
    __idtr.base_ = (uint32_t)__idt;
    idtr_t idtr_value;
    __asm__  volatile ("lidt %k1\n\t"
        "sidt %0" : "=m"(idtr_value) : "m"(__idtr));
    klog_write("[DEBUG] idt: 0x%x, idtr: 0x%x\n", __idt, idtr_value.base_);

    syscall_init();
}
