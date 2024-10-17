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
     set_isr_entry(&__isr[ISR32_TIMER], (isr_t)timer);

    for (uint32_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(&__idt[i], PL_KERN, INTER_GATE, (uint32_t)isr_part1[i]);

    // load idtr
    __idtr.limit_ = sizeof(__idt) - 1;
    __idtr.base_ = (uint32_t)__idt;
    idtr_t idtr_value;
    __asm__  volatile ("lidt %k1\n\t"
        "sidt %0" : "=m"(idtr_value) : "m"(__idtr));
    printf("[DEBUG] idt: 0x%x, idtr: 0x%x\n", __idt, idtr_value.base_);
}
