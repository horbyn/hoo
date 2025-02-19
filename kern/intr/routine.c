#include "routine.h"
#include "kern/panic.h"
#include "kern/sched/tasks.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "user/lib.h"

/**
 * @brief Intel 保留的异常名称
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
 * @brief 默认的 ISR
 */
void
isr_default(void) {
    uint32_t vec = 0;
    __asm__ ("movl 56(%%ebp), %0": "=a"(vec) ::);
    // FIXME：bochs 经常抛出 0x26 异常不确定是哪里有问题
    if (vec == 0x26)    return;
    // spurious interrupt 不是 IRQ 信号
    if (vec == 0x27 || vec == 0x2f)    return;

    uint32_t arr_idx = vec <= (NELEMS(__exception_names) - 2) ?
        vec : (NELEMS(__exception_names) - 2);

    panic(__exception_names[arr_idx]);
}

/**
 * @brief 缺页异常 ISR
 */
void
page_fault(void) {
    void *linear_addr = 0;
    __asm__ ("movl %%cr2, %0": "=a"(linear_addr) ::);

    uint32_t err = 0;
    // 错误码从 60(%%ebp) 处获取是因为
    // +4: ebp + 4 到达返回地址
    // +(4 * 12): 跨越中断栈
    // +4: CPU 自动压入的中断向量码
    // +4: 我们想要获取的错误码
    __asm__ ("movl 60(%%ebp), %0": "=a"(err) ::);

    // C.O.W
    if ((err & PGFLAG_RW) == PGFLAG_RW) {
        pcb_t *pcb = get_current_pcb();
        pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

        void *linear_addr_pa = phy_alloc_page();
        void *temp_va = vir_alloc_pages(&pcb->vmngr_, 1, pcb->break_);
        set_mapping(temp_va, linear_addr_pa, flags);
        memmove(temp_va, linear_addr, PGSIZE);
        vir_release_pages(&pcb->vmngr_, temp_va, false);

        pgelem_t *pte = (pgelem_t *)GET_PTE(linear_addr);
        (*pte) = (pgelem_t)linear_addr_pa | flags;
    }

    return;
}

/**
 * @brief 时间片 ISR
 */
void
timer(void) {
    scheduler();
}
