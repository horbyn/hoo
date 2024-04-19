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
 * @brief create a kernel thread (stack always assumed 4MB)
 * if ring-3 stack specified, then is a user mode thread
 * 
 * @param r0_top top of ring 0 stack
 * @param r3_top top of ring 3 stack
 * @param entry  thread entry
 */
void
thread_create(uint8_t *r0_top, uint8_t *r3_top, void *entry) {

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
    uint8_t *pstack = r0_top + PGSIZE;

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
        *((uint32_t *)pstack) = (uint32_t)r3_top + PGSIZE;
    }

    pstack -= sizeof(istackcpu_t);
    istackcpu_t *workercpu = (istackcpu_t *)pstack;

    pstack -= sizeof(istackos_t);
    istackos_t *workeros = (istackos_t *)pstack;

    pstack -= sizeof(tstack_t);
    tstack_t *workerth = (tstack_t *)pstack;

    // setup the thread context
    // the new task will enable interrupt
    workercpu->eflags_ = EFLAGS_IF;
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

    // setup the thread pcb which lies at the bottom of the stack
    pcb_t *pcb = (pcb_t *)r0_top;
    pgelem_t *idle_pd = get_idle_pgdir();
    pcb->tid_ = allocate_tid();
    const uint32_t PAGE4 = 4;
    void *va = vir_alloc_pages(get_idle_pcb(), PAGE4), *pa_pdir = null;
    for (uint32_t i = 0; i < PAGE4; ++i) {
        void *pa = phy_alloc_page();
        if (i == 0)    pa_pdir = pa;
        set_mapping(idle_pd, (uint32_t)va + i * PGSIZE, (uint32_t)pa);
    }
    pcb_set(pcb, (uint32_t *)pstack, (uint32_t *)((uint32_t)r0_top + PGSIZE),
        pcb->tid_, va, pa_pdir, va + PGSIZE, va + 2 * PGSIZE, va + 3 * PGSIZE, TIMETICKS);

    // setup page directory table
    for (uint32_t i = PD_INDEX(KERN_HIGH_MAPPING); i < (PGSIZE / sizeof(uint32_t)); ++i)
        ((pgelem_t *)va)[i] = ((idle_pd[i] & 0xfffff000) | (pgelem_t)PGENT_US | PGENT_RW | PGENT_PS);
    ((pgelem_t *)va)[(PGSIZE / sizeof(uint32_t)) - 1] =
        (pgelem_t)pa_pdir | PGENT_US | PGENT_RW | PGENT_PS;

    // setup to the ready queue waiting to execute
    node_set(__mdata_node + pcb->tid_, pcb, null);
    task_ready(__mdata_node + pcb->tid_);
}
