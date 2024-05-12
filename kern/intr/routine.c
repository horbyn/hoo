/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "routine.h"

/**
 * @brief intel reserved exception names
 */
static char *__exception_names[] = {
    "#DE, Division Error, Fault, No error-code",            // #0
    "#DB, Debug, Fault/Trap, No error-code",                // #1
    ", Non-maskable interrupt, Interrupt, No error-code",   // #2
    "#BP, Breakpoint, Trap, No error-code",                 // #3
    "#OF, Overflow, Trap, No error-code",                   // #4
    "#BR, Bound Range Exceeded, Fault, No error-code",      // #5
    "#UD, Invalid Opcode, Fault, No error-code",            // #6
    "#NM, Device Not Available, Fault, No error-code",      // #7
    "#DF, Double Fault, Abort, error-code",                 // #8
    ", Coprocessor Segment Overrun, Fault, No error-code",  // #9
    "#TS, Invalid TSS, Fault, error-code",                  // #10
    "#NP, Segment Not Present, Fault, error-code",          // #11
    "#SS, Stack-Segment Fault, Fault, error-code",          // #12
    "#GP, General Protection Fault, Fault, error-code",     // #13
    "#PF, Page Fault, Fault, error-code",                   // #14
    ", RESERVED, , ",                                       // #15
    "#MF, x87 Floating-Point, Fault, no error-code",        // #16
    "#AC, Alignment Check, Fault, error-code",              // #17
    "#MC, Machine Check, Abort, no error-code",             // #18
    "#XM/#XF, SIMD Floating-Point, Fault, no error-code",   // #19
    "#VE, Virtualization, Fault, no error-code",            // #20
    "#CP, Control Protection, Fault, error-code",           // #21
    ", RESERVED, , ",                                       // #22
    ", RESERVED, , ",                                       // #23
    ", RESERVED, , ",                                       // #24
    ", RESERVED, , ",                                       // #25
    ", RESERVED, , ",                                       // #26
    ", RESERVED, , ",                                       // #27
    "#HV, Hypervisor Injection, Fault, no error-code",      // #28
    "#VC, VMM Communication, Fault, error-code",            // #29
    "#SX, Security, Fault, error-code",                     // #30
    ", RESERVED, , ",                                       // #31
    0
};

/**
 * @brief default isr routine
 */
void
isr_default(void) {
    uint32_t vec = 0;
    __asm__ ("movl 56(%%ebp), %0": "=a"(vec) ::);
    // not sure if it’s a problem with bochs that often throws exception 0x26
    if (vec == 0x26)    return;
    // spurious interrupt is not interrupt
    if (vec == 0x27 || vec == 0x2f)    return;

    uint32_t arr_idx = vec <= (NELEMS(__exception_names) - 2) ?
        vec : (NELEMS(__exception_names) - 2);

    panic(__exception_names[arr_idx]);
}

/**
 * @brief page fault handler
 */
void
page_fault(void) {
    uint32_t linear_addr = 0;
    __asm__ ("movl %%cr2, %0": "=a"(linear_addr) ::);
    if (linear_addr == 0)    return;

    uint32_t err = 0;
    __asm__ ("movl 60(%%ebp), %0": "=a"(err) ::);
    static const uint32_t FLAG_WR = 0x10;

    if ((err & FLAG_WR) == FLAG_WR) {
        // C.O.W

        //void *va = vir_alloc_pages(get_hoo_pcb(), 1);
        //if (va < KERN_AVAIL_VMBASE)    panic("page_fault(): bug");
        //void *pa = phy_alloc_page();
        //set_mapping(get_hoo_pgdir(), (uint32_t)va, (uint32_t)pa,
        //    PGENT_US | PGENT_RW | PGENT_PS);

        //memmove(va, (void *)linear_addr, PGSIZE);
    }

    return;
}

/**
 * @brief handling system timer
 */
void
timer(void) {
    scheduler();
}
