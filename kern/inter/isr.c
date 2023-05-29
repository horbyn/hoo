/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "inte.h"

void
info(uint32_t isr_ebp, uint32_t isr_to_part2,
uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp,
uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax,
uint32_t ss,  uint32_t gs,  uint32_t fs,  uint32_t es,
uint32_t ds,  uint32_t idx, uint32_t ecode, uint32_t eip,
uint32_t cs,  uint32_t eflags) {
    set_cursor(0, 0);

    kprintf(">>>>> The interruption vector "
        "%d  <<<<<\n\n", idx);
    kprintf("\nEFLAGS = %x\nCS = %x\nEIP = %x"
        "\nECODE = %x\n", eflags, (cs & 0xffff),
        eip, ecode);
    kprintf("DS = %x\nES = %x\nFS = %x\nGS = %x\n"
        "SS = %x\n", (ds & 0xffff), (es & 0xffff),
        (fs & 0xffff), (gs & 0xffff), (ss & 0xffff));
    kprintf("EAX = %x\nECX = %x\nEDX = %x\nEBX = %x\n"
        "ESP = %x\nEBP = %x\nESI = %x\nEDI = %x\n",
        eax, ecx, edx, ebx, esp, ebp, esi, edi);
    kprintf("ISR2PART2 = %x\nISP_EBP = %x\n",
        isr_to_part2, isr_ebp);
}

void
isr_default(void) {
    __asm__ volatile ("call info\n\t"::);
    __asm__ volatile ("hlt\n\t"::);
}

void
divide_error(void) {
    __asm__ volatile ("call info\n\t"::);
    __asm__ volatile ("hlt\n\t"::);
}

void
timer(void) {
    static int ticker = 0;
    set_cursor(0, 0);
    kprint_int(ticker++);
}
