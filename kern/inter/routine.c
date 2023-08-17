/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
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
 * @brief display some info on cpu context when mechine deep into panic
 * 
 * @param isr_ebp origin ebp
 * @param isr_to_part2 index of isr array
 * @param edi 
 * @param esi 
 * @param ebp 
 * @param esp 
 * @param ebx 
 * @param edx 
 * @param ecx 
 * @param eax 
 * @param ss 
 * @param gs 
 * @param fs 
 * @param es 
 * @param ds 
 * @param idx 
 * @param ecode error code
 * @param eip 
 * @param cs 
 * @param eflags 
 */
void
info(uint32_t isr_ebp, uint32_t isr_to_part2, uint32_t edi,
uint32_t esi, uint32_t ebp, uint32_t esp, uint32_t ebx,
uint32_t edx, uint32_t ecx, uint32_t eax, uint32_t ss,
uint32_t gs,  uint32_t fs,  uint32_t es, uint32_t ds,
uint32_t idx, uint32_t ecode, uint32_t eip, uint32_t cs,
uint32_t eflags) {
    clear_screen();
    set_cursor(0, 0);

    kprintf(">>>>> %s <<<<<\n\n", __exception_names[idx]);
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

/**
 * @brief default isr routine
 */
void
isr_default(void) {
    __asm__ volatile ("call info\n\t"::);
    __asm__ volatile ("hlt\n\t"::);
}

/**
 * @brief handling division of zero
 */
void
divide_error(void) {
    __asm__ volatile ("call info\n\t"::);
    __asm__ volatile ("hlt\n\t"::);
}

/**
 * @brief handling system timer
 */
void
timer(void) {
    // change two tasks queue
    node_t *cur = queue_pop(&__queue_running);
    node_t *next = queue_pop(&__queue_ready);

    queue_push(&__queue_running, next);
    queue_push(&__queue_ready, cur);

    // update tss
    __tss.ss0_ = DS_SELECTOR_KERN;
    __tss.esp0_ = (uint32_t)((pcb_t *)next->data_)->stack0_;

    scheduler(cur, next);
}
