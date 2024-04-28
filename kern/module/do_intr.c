/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "do_intr.h"

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
    //set_isr_entry(&__isr[ISR14_PAGEFAULT], (isr_t)page_fault);
    set_isr_entry(&__isr[ISR32_TIMER], (isr_t)timer);
    set_isr_entry(&__isr[ISR46_HARD1], (isr_t)isr_default);
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
    __asm__ __volatile__ ("lidt %k0\n\t" : : "m"(__idtr));
}
