/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
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
 * @brief initialize isr and idt by default
 */
void
kinit_isr_idt(void) {
    // set default __isr[]
    for (uint32_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_isr_entry(&__isr[i], (isr_t)isr_default);

    // specific isr routines
    set_isr_entry(&__isr[ISR32_TIMER], (isr_t)timer);
    set_isr_entry(&__isr[ISR33_KEYBOARD], (isr_t)ps2_intr);
    set_isr_entry(&__isr[ISR46_HARD1], (isr_t)ata_irq_intr);
    set_isr_entry(&__isr[ISR128_SYSCALL], (isr_t)syscall);

    // set __idt[]
    // all the idts point to the default handling;
    // after that, idt #128 that corresponding to isr #34 points to system call
    // which need dpl 3
    for (uint32_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(&__idt[i], PL_KERN, INTER_GATE, (uint32_t)isr_part1[i]);
    set_idt_entry(&__idt[ISR128_SYSCALL], PL_USER, TRAP_GATE,
        (uint32_t)isr_part1[ISR128_SYSCALL]);

    // load idtr
    __idtr.limit_ = sizeof(__idt) - 1;
    __idtr.base_ = (uint32_t)__idt;
    idtr_t idtr_value;
    __asm__  volatile ("lidt %k1\n\t"
        "sidt %0" : "=m"(idtr_value) : "m"(__idtr));
    klog_write("[DEBUG] idt: 0x%x, idtr: 0x%x\n", __idt, idtr_value.base_);

    syscall_init();
}
