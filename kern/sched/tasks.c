/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

static node_t __mdata_node[MAX_TASKS_AMOUNT];
// serially access thread id
static spinlock_t __spinlock_alloc_tid;
// serially access task queues
static spinlock_t __spinlock_tasks_queue;
static queue_t __queue_ready, __queue_running;
static tid_t __global_tid;

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    wait(&__spinlock_tasks_queue);
    node_t *cur = queue_front(&__queue_running);
    signal(&__spinlock_tasks_queue);
    if (!cur)
        panic("get_current_pcb(): no current task");

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    if (!cur_pcb)
        panic("get_current_pcb(): current task is null");

    return cur_pcb;
}

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    bzero(__mdata_node, sizeof(__mdata_node));
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    spinlock_init(&__spinlock_alloc_tid);
    spinlock_init(&__spinlock_tasks_queue);

    // metadata
    __attribute__((aligned(4096))) static uint8_t mdata_vspace[PGSIZE] = { 0 },
        mdata_node[PGSIZE] = { 0 }, mdata_vaddr[PGSIZE] = { 0 };

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_t *hoo_pcb = get_hoo_pcb();
    pcb_set(null, (uint32_t *)STACK_HOO_RING0, hoo_pcb->tid_, get_hoo_pgdir(),
        (void *)(V2P(get_hoo_pgdir())), mdata_vspace, mdata_node, mdata_vaddr, TIMETICKS);
    node_set(__mdata_node + hoo_pcb->tid_, hoo_pcb, null);
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_running, __mdata_node + hoo_pcb->tid_, TAIL);
    signal(&__spinlock_tasks_queue);

    vir_alloc_pages(hoo_pcb, (KERN_HIGH_MAPPING + MM_BASE) / PGSIZE);
}

/**
 * @brief allocate the thread id
 * 
 * @return thread id
 */
tid_t
allocate_tid() {
    tid_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = ++__global_tid;
    signal(&__spinlock_alloc_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("allocate_tid(): thread id overflows");
    return temp;
}

/**
 * @brief scheduler two thread
 */
void
scheduler() {
    if (test(&__spinlock_tasks_queue))    return;

    // to check whether ticks expired
    // we just need the first queue node
    wait(&__spinlock_tasks_queue);
    node_t *cur = queue_front(&__queue_running);
    if (cur) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            signal(&__spinlock_tasks_queue);
            return;
        } else    ((pcb_t *)cur->data_)->ticks_ = TIMETICKS;
    }

    node_t *next = queue_pop(&__queue_ready);
    if (next) {
        cur = queue_pop(&__queue_running);
        // only change tasks when the `cur` task exists
        if (cur)
            queue_push(&__queue_ready, cur, TAIL);
        queue_push(&__queue_running, next, TAIL);

        // update tss
        tss_t *tss = get_hoo_tss();
        tss->ss0_ = DS_SELECTOR_KERN;
        tss->esp0_ = (uint32_t)((pcb_t *)next->data_)->stack0_;

        signal(&__spinlock_tasks_queue);
        switch_to(cur, next);
    }

    signal(&__spinlock_tasks_queue);
}

/**
 * @brief make a task get ready
 * 
 * @param task the node of a task pcb object
 */
void
task_ready(node_t *task) {
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_ready, task, TAIL);
    signal(&__spinlock_tasks_queue);
}

/**
 * @brief setup the ring0 stack of hoo
 * 
 * @param r0_top top of ring 0 stack
 * @param r3_top top of ring 3 stack
 * @param entry  thread entry
 * @return ring0 stack pointer after setup
 */
static uint32_t *
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

    return (uint32_t *)pstack;
}

/**
 * @brief setup the first ring3 thread, idle
 */
void
init_idle(void) {
    pcb_t *hoo_pcb = get_hoo_pcb();
    static const uint32_t PAGES = 7;
    void *va = vir_alloc_pages(hoo_pcb, PAGES);

    // setup the idle page directory table (1 page)
    void *idle_pgdir_pa = phy_alloc_page();
    void *hoo_temp_pgdir_va = va;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_pgdir_va,
        (uint32_t)idle_pgdir_pa);

    // setup the idle page table (1 page)
    void *idle_pg_pa = phy_alloc_page();
    void *hoo_temp_pg_va = va + PGSIZE * 1;
    set_mapping(
        hoo_pcb->pdir_va_, (uint32_t)hoo_temp_pg_va, (uint32_t)idle_pg_pa);

    // setup the idle ring0 stack (1 page)
    void *idle_ring0_pa = phy_alloc_page();
    void *hoo_temp_ring0_va = va + PGSIZE * 2;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_ring0_va,
        (uint32_t)idle_ring0_pa);

    // setup the idle ring3 stack (1 page)
    void *idle_ring3_pa = phy_alloc_page();
    void *hoo_temp_ring3_va = va + PGSIZE * 3;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_ring3_va,
        (uint32_t)idle_ring3_pa);

    // setup the idle metadata (3 pages)
    void *idle_vspace_pa = phy_alloc_page();
    void *hoo_temp_vspace_va = va + PGSIZE * 4;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_vspace_va,
        (uint32_t)idle_vspace_pa);
    void *idle_node_pa = phy_alloc_page();
    void *hoo_temp_node_va = va + PGSIZE * 5;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_node_va,
        (uint32_t)idle_node_pa);
    void *idle_vaddr_pa = phy_alloc_page();
    void *hoo_temp_vaddr_va = va + PGSIZE * 6;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_vaddr_va,
        (uint32_t)idle_vaddr_pa);

    // setup the idle thread
    hoo_temp_ring0_va = (void *)((uint32_t)hoo_temp_ring0_va + PGSIZE);
    hoo_temp_ring3_va = (void *)((uint32_t)hoo_temp_ring3_va + PGSIZE);
    uint32_t *cur_stack = setup_ring0_stack(
        hoo_temp_ring0_va, hoo_temp_ring3_va, idle);

    tid_t idle_tid = allocate_tid();
    pcb_set(cur_stack, hoo_temp_ring0_va, idle_tid, hoo_temp_pgdir_va,
        idle_pgdir_pa, hoo_temp_vspace_va, hoo_temp_node_va, hoo_temp_vaddr_va,
        TIMETICKS);
    pcb_t *idle_pcb = pcb_get(idle_tid);
    vir_alloc_pages(idle_pcb, PAGES);

    // setup idle page directory table
    pgelem_t *hoo_pgdir = hoo_pcb->pdir_va_;
    pgelem_t flag = (pgelem_t)PGENT_US | PGENT_RW | PGENT_PS;
    for (uint32_t i = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING);
        i < (PGSIZE / sizeof(uint32_t) - 1); ++i)
        ((pgelem_t *)hoo_temp_pgdir_va)[i] = hoo_pgdir[i];
    ((pgelem_t *)hoo_temp_pgdir_va)[0] = (pgelem_t)hoo_temp_pg_va | flag;

    // setup idle page table
    ((pgelem_t *)hoo_temp_pg_va)[0] = (pgelem_t)idle_pgdir_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[1] = (pgelem_t)idle_ring0_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[2] = (pgelem_t)idle_ring3_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[3] = (pgelem_t)idle_vspace_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[4] = (pgelem_t)idle_node_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[5] = (pgelem_t)idle_vaddr_pa | flag;

    node_set(__mdata_node + idle_tid, idle_pcb, null);
    task_ready(__mdata_node + idle_tid);

    // release
    vir_release_pages(hoo_pcb, va);
}
