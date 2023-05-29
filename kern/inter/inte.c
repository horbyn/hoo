/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "inte.h"

// IDTR
static idtr_t idtr;

// IDT[]
__attribute__((aligned(0x8)))
static idt_entry_t idt[IDT_ENTRIES_NUM];

// ISR[]
__attribute__((aligned(0x4)))
isr_t isr[IDT_ENTRIES_NUM];

void
init_interrupt(void) {
    // set isr[0..IDT_INTEL_NUM]
    set_isr_entry(0, (uint32_t)divide_error);
    set_isr_entry(32, (uint32_t)timer);

    // set rest isr[]
    for (size_t i = IDT_INTEL_NUM; i < IDT_ENTRIES_NUM; ++i)
        set_isr_entry(i, (uint32_t)isr_default);

    // set idt[]
    for (size_t i = 0; i < IDT_INTEL_NUM; ++i)
        set_idt_entry(i, (uint32_t)isr_part1[i]);
    for (size_t i = IDT_INTEL_NUM; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(i, (uint32_t)0);

    // load idtr
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)idt;
    __asm__ __volatile__ ("lidt %k0\n\t" : : "m"(idtr));
}

void
set_idt_entry(int id, uint32_t addr) {
    // 0x80 is `P/DPL/0 == 1/00/0`
    uint8_t attr = (0x80 | GATE_INTERRUPT);

    idt[id].isr_low = (uint16_t)addr;
    idt[id].selector = CS_SELECTOR;
    idt[id].reserved = 0;
    idt[id].attributes = attr;
    idt[id].isr_high = (uint16_t)(addr >> 16);
}

void
set_isr_entry(int id, uint32_t addr) {
    isr[id] = (isr_t)addr;
}
