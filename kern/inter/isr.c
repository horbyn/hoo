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

    // set timer(), syscall()
    set_isr_entry(ISR32_TIMER, (uint32_t)timer);
    set_isr_entry(ISR38_FLOPPY, (uint32_t)floppy_driver);
    set_isr_entry(ISR128_SYSCALL, (uint32_t)syscall);

    // set __idt[]
    // all the idts point to the default handling;
    // after that, idt #128 that corresponding to isr #34 points to system call
    // which need dpl 3
    for (size_t i = 0; i < IDT_ENTRIES_NUM; ++i)
        set_idt_entry(PL_KERN, INTER_GATE, i, (uint32_t)isr_part1[i]);
    set_idt_entry(PL_USER, TRAP_GATE, ISR128_SYSCALL, (uint32_t)isr_part1[ISR128_SYSCALL]);

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
set_idt_entry(privilege_t dpl, gatedesc_t gate, int id, uint32_t addr) {
    // the high 4 bytes are `P/DPL/0`
    uint8_t attr = (1 << 7 | dpl << 5 | (uint8_t)gate);

    __idt[id].isr_low = (uint16_t)addr;
    __idt[id].selector = CS_SELECTOR_KERN;
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
