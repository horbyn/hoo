/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "isr.h"

static idtr_t __idtr;                                       // IDTR
__attribute__((aligned(0x8))) static
    idt_entry_t __idt[IDT_ENTRIES_NUM];                     // IDT[]
__attribute__((aligned(0x4)))
    isr_t __isr[IDT_ENTRIES_NUM];                           // ISR[]

/**
 * @brief initialize isr and idt by default
 */
void
init_isr_idt(void) {
    // set default __isr[]
    for (size_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_isr_entry(i, (uint32_t)isr_default);

    // set __isr[0]; __isr[IDT_NEED_TO_INIT_NUM - 1]
    set_isr_entry(0, (uint32_t)divide_error);
    set_isr_entry(IDT_NEED_TO_INIT_NUM - 1, (uint32_t)timer);

    // set __idt[]
    for (size_t i = 0; i < IDT_NEED_TO_INIT_NUM; ++i)
        set_idt_entry(i, (uint32_t)isr_part1[i]);
    for (size_t i = IDT_NEED_TO_INIT_NUM; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(i, (uint32_t)isr_part1[IDT_NEED_TO_INIT_NUM]);

    // load idtr
    __idtr.limit = sizeof(__idt) - 1;
    __idtr.base = (uint32_t)__idt;
    __asm__ __volatile__ ("lidt %k0\n\t" : : "m"(__idtr));
}

/**
 * @brief Set the idt entry
 * 
 * @param id   index of idt array
 * @param addr routine entry
 */
void
set_idt_entry(int id, uint32_t addr) {
    // 0x80 is `P/DPL/0 == 1/00/0`
    uint8_t attr = (0x80 | GATE_INTERRUPT);

    __idt[id].isr_low = (uint16_t)addr;
    __idt[id].selector = CS_SELECTOR;
    __idt[id].reserved = 0;
    __idt[id].attributes = attr;
    __idt[id].isr_high = (uint16_t)(addr >> 16);
}

/**
 * @brief Set the isr entry
 * 
 * @param id   index of isr array
 * @param addr routine entry
 */
void
set_isr_entry(int id, uint32_t addr) {
    __isr[id] = (isr_t)addr;
}
