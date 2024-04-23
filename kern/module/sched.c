/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sched.h"

static node_t __mdata_node[MAX_TASKS_AMOUNT];

/**
 * @brief kernel initializes the tasks system
 */
void
kinit_tasks_system(void) {
    init_tasks_system();
    bzero(__mdata_node, sizeof(__mdata_node));
}

/**
 * @brief setup the ring0 stack of idle
 * 
 * @param r0_top top of ring 0 stack
 * @param r3_top top of ring 3 stack
 * @param entry  thread entry
 */
static void
setup_ring0_stack(void *r0_top, void *r3_top, void *entry) {

    /*
     ************************************
     * kernel stack of a thread :       *
     * ┌───────────────────────────────┐*
     * │     (4B) DIED INSTRUCTION     │*
     * ├───────────────────────────────┤*
     * │    (4B) user thread entry     │*
     * ├───────────────────────────────┤*
     * │   (4B) user ring3 stack top   │*
     * ├───────────────────────────────┤*
     * │  interrupt stack used by cpu  │*
     * ├───────────────────────────────┤*
     * │  interrupt stack used by os   │*
     * ├───────────────────────────────┤*
     * │          thread stack         │*
     * ├───────────────────────────────┤*
     * │                               │*
     * │                               │*
     * │             stack             │*
     * │                               │*
     * ├───────────────────────────────┤*
     * │              pcb              │*
     * └───────────────────────────────┘*
     ************************************/

    // setup the kernel stack
    uint8_t *pstack = (uint8_t *)r0_top;

    // always located on the top of new task stack that the `esp`
    // pointed to when the new task completes its initialization
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = DIED_INSTRUCTION;

    if (r3_top) {
        // user mode entry
        pstack -= sizeof(uint32_t);
        *((uint32_t *)pstack) = (uint32_t)entry;

        // user mode stack
        pstack -= sizeof(uint32_t);
        *((uint32_t *)pstack) = (uint32_t)r3_top;
    }

    pstack -= sizeof(istackcpu_t);
    istackcpu_t *workercpu = (istackcpu_t *)pstack;

    pstack -= sizeof(istackos_t);
    istackos_t *workeros = (istackos_t *)pstack;

    pstack -= sizeof(tstack_t);
    tstack_t *workerth = (tstack_t *)pstack;

    // setup the thread context
    // the new task will enable interrupt
    workercpu->eflags_ = r3_top ? 0 : EFLAGS_IF;
    workercpu->oldcs_  = CS_SELECTOR_KERN;
    workercpu->oldeip_ = r3_top ?
        (uint32_t *)mode_ring3 : (uint32_t *)entry;
    workercpu->errcode_ = 0;
    workercpu->vec_ = 0;
    workeros->ds_ = DS_SELECTOR_KERN;
    workeros->es_ = DS_SELECTOR_KERN;
    workeros->fs_ = DS_SELECTOR_KERN;
    workeros->gs_ = DS_SELECTOR_KERN;
    workeros->eax_ = 0;
    workeros->ecx_ = 0;
    workeros->edx_ = 0;
    workeros->ebx_ = 0;
    // skip the 4 segment regs
    workeros->esp_ = (uint32_t)(((uint32_t *)workercpu) - 4);
    workeros->ebp_ = 0;
    workeros->esi_ = 0;
    workeros->edi_ = 0;
    workerth->retaddr_ = isr_part3;

    // metadata
    __attribute__((aligned(4096))) static uint8_t mdata_vspace[PGSIZE] = { 0 },
        mdata_node[PGSIZE] = { 0 }, mdata_vaddr[PGSIZE] = { 0 };

    pcb_t *idle_pcb = get_idle_pcb();
    pcb_set(idle_pcb, (uint32_t *)pstack, (uint32_t *)STACK_IDLE_RING0,
        allocate_tid(), get_idle_pgdir(), (void *)V2P(get_idle_pgdir()),
        mdata_vspace, mdata_node, mdata_vaddr, TIMETICKS);
    vir_alloc_pages(idle_pcb, KERN_AVAIL_VMBASE / PGSIZE);
}

/**
 * @brief make the idle thread to enter ring3
 */
void
idle_enter_ring3(void) {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack
    setup_ring0_stack((void *)STACK_IDLE_RING0, (void *)STACK_IDLE_RING3, idle);

    // setup to the ready queue waiting to execute
    pcb_t *idle_pcb = get_idle_pcb();
    node_set(__mdata_node + idle_pcb->tid_, idle_pcb, null);
    task_ready(__mdata_node + idle_pcb->tid_);
}
