#include "inte.h"

void
info(uint32_t isr_ebp, uint32_t isr_to_part2,
uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp,
uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax,
uint32_t ss,  uint32_t gs,  uint32_t fs,  uint32_t es,
uint32_t ds,  uint32_t idx, uint32_t ecode, uint32_t eip,
uint32_t cs,  uint32_t eflags) {
    set_cursor(0, 0);

    kprint_str(">>>>> The interruption vector ");
    kprint_int(idx);
    kprint_str(" <<<<<\n\n");

    kprint_str("\nEFLAGS = 0x");
    kprint_hex(eflags);
    kprint_str("\nCS = ");
    kprint_hex((cs & 0xffff));
    kprint_str("\nEIP = ");
    kprint_hex(eip);
    kprint_str("\nECODE = ");
    kprint_hex(ecode);

    kprint_str("\nDS = ");
    kprint_hex((ds & 0xffff));
    kprint_str("\nES = ");
    kprint_hex((es & 0xffff));
    kprint_str("\nFS = ");
    kprint_hex((fs & 0xffff));
    kprint_str("\nGS = ");
    kprint_hex((gs & 0xffff));
    kprint_str("\nSS = ");
    kprint_hex((ss & 0xffff));

    kprint_str("\nEAX = ");
    kprint_hex(eax);
    kprint_str("\nECX = ");
    kprint_hex(ecx);
    kprint_str("\nEDX = ");
    kprint_hex(edx);
    kprint_str("\nEBX = ");
    kprint_hex(ebx);
    kprint_str("\nESP = ");
    kprint_hex(esp);
    kprint_str("\nEBP = ");
    kprint_hex(ebp);
    kprint_str("\nESI = ");
    kprint_hex(esi);
    kprint_str("\nEDI = ");
    kprint_hex(edi);

    kprint_str("\nISR2PART2 = ");
    kprint_hex(isr_to_part2);
    kprint_str("\nISR_EBP = ");
    kprint_hex(isr_ebp);
    kprint_char('\n');
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
