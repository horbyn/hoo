/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

static node_t __mdata_node[MAX_TASKS_AMOUNT];
// serially access task queues
static spinlock_t __spinlock_tasks_queue;
// serially access metadata
static spinlock_t __spinlock_mdata_node;
static queue_t __queue_ready, __queue_running;
static list_t __list_sleeping;
static const uint32_t IDLE_PAGES = 6;
static void *idle_pgdir_va;

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
 * @brief get metadata node
 * 
 * @param tid thread id
 * @return node object pointer
 */
static node_t *
mdata_alloc_node(tid_t tid) {
    node_t *temp = null;
    wait(&__spinlock_mdata_node);
    temp = __mdata_node + tid;
    signal(&__spinlock_mdata_node);
    return temp;
}

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    init_pcb_system();
    bzero(__mdata_node, sizeof(__mdata_node));
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    list_init(&__list_sleeping, false);
    spinlock_init(&__spinlock_tasks_queue);
    spinlock_init(&__spinlock_mdata_node);

    // metadata
    __attribute__((aligned(4096))) static uint8_t mdata_vspace[PGSIZE] = { 0 },
        mdata_node[PGSIZE] = { 0 }, mdata_vaddr[PGSIZE] = { 0 };

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_t *hoo_pcb = get_hoo_pcb();
    pcb_set(null, (uint32_t *)STACK_HOO_RING0, hoo_pcb->tid_, get_hoo_pgdir(),
        (void *)(V2P(get_hoo_pgdir())), mdata_vspace, mdata_node, mdata_vaddr,
        null, TIMETICKS, null);
    node_t *n = mdata_alloc_node(hoo_pcb->tid_);
    node_set(n, hoo_pcb, null);
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_running, n, TAIL);
    signal(&__spinlock_tasks_queue);

    vir_alloc_pages(hoo_pcb, (KERN_HIGH_MAPPING + MM_BASE) / PGSIZE);
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
    if (!((pcb_t *)cur->data_)->sleeplock_ && cur) {
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
        if (cur) {
            if (((pcb_t *)cur->data_)->sleeplock_)
                list_insert(&__list_sleeping, cur, LSIDX_AFTAIL(&__list_sleeping));
            else    queue_push(&__queue_ready, cur, TAIL);
        }
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
 * @param r0_hoo top of hoo thread ring 0 stack
 * @param r0_idle top of idle thread ring 0 stack
 * @param r3_idle top of idle thread ring 3 stack
 * @param entry  thread entry
 * @return ring0 stack pointer after setup
 */
static uint32_t *
setup_idle_ring0_stack(void *r0_hoo, void *r0_idle, void *r3_idle, void *entry) {

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
    uint8_t *pstack = (uint8_t *)r0_hoo;

    // always located on the top of new task stack that the `esp`
    // pointed to when the new task completes its initialization
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (DIED_INSTRUCTION + KERN_HIGH_MAPPING);

    // user mode entry
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)entry;

    // user mode stack
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)r3_idle;

    pstack -= sizeof(istackcpu_t);
    istackcpu_t *workercpu = (istackcpu_t *)pstack;

    pstack -= sizeof(istackos_t);
    istackos_t *workeros = (istackos_t *)pstack;

    pstack -= sizeof(tstack_t);
    tstack_t *workerth = (tstack_t *)pstack;

    // setup the thread context
    // the new task will enable interrupt
    workercpu->eflags_ = 0;
    workercpu->oldcs_  = CS_SELECTOR_KERN;
    workercpu->oldeip_ = (uint32_t *)mode_ring3;
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
    uint32_t offset = (uint32_t)r0_hoo - (uint32_t)(((uint32_t *)workercpu) - 4);
    workeros->esp_ = (uint32_t)r0_idle - offset;
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
idle_init(void) {
    pcb_t *hoo_pcb = get_hoo_pcb();

    // setup the idle page directory table (1 page)
    void *idle_pgdir_pa = phy_alloc_page();
    idle_pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)idle_pgdir_va,
        (uint32_t)idle_pgdir_pa);

    // setup the idle page table (1 page)
    void *va = vir_alloc_pages(hoo_pcb, IDLE_PAGES);
    void *idle_pg_pa = phy_alloc_page();
    void *hoo_temp_pg_va = va;
    set_mapping(
        hoo_pcb->pdir_va_, (uint32_t)hoo_temp_pg_va, (uint32_t)idle_pg_pa);

    // setup the idle ring0 stack (1 page)
    void *idle_ring0_pa = phy_alloc_page();
    void *hoo_temp_ring0_va = va + PGSIZE;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_ring0_va,
        (uint32_t)idle_ring0_pa);

    // setup the idle ring3 stack (1 page)
    void *idle_ring3_pa = phy_alloc_page();
    void *hoo_temp_ring3_va = va + PGSIZE * 2;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_ring3_va,
        (uint32_t)idle_ring3_pa);

    // setup the idle metadata (3 pages)
    void *idle_vspace_pa = phy_alloc_page();
    void *hoo_temp_vspace_va = va + PGSIZE * 3;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_vspace_va,
        (uint32_t)idle_vspace_pa);
    void *idle_node_pa = phy_alloc_page();
    void *hoo_temp_node_va = va + PGSIZE * 4;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_node_va,
        (uint32_t)idle_node_pa);
    void *idle_vaddr_pa = phy_alloc_page();
    void *hoo_temp_vaddr_va = va + PGSIZE * 5;
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_temp_vaddr_va,
        (uint32_t)idle_vaddr_pa);

    // setup the idle thread
    static const pgelem_t IDLE_PGDIR_VA = 0xfffff000;
    static const pgelem_t IDLE_RING0_VA = 0;
    static const pgelem_t IDLE_RING3_VA = IDLE_RING0_VA + PGSIZE;
    static const pgelem_t IDLE_VS_VA = IDLE_RING3_VA + PGSIZE;
    static const pgelem_t IDLE_NODE_VA = IDLE_VS_VA + PGSIZE;
    static const pgelem_t IDLE_VADDR_VA = IDLE_NODE_VA + PGSIZE;

    hoo_temp_ring0_va = (void *)((uint32_t)hoo_temp_ring0_va + PGSIZE);
    uint32_t *cur_stack = setup_idle_ring0_stack(hoo_temp_ring0_va,
        (void *)(IDLE_RING0_VA + PGSIZE), (void *)(IDLE_RING3_VA + PGSIZE), idle);

    tid_t idle_tid = TID_IDLE;
    cur_stack = (uint32_t *)(PG_OFFSET((uint32_t)cur_stack) | IDLE_RING0_VA);
    pcb_set(cur_stack, (uint32_t *)(IDLE_RING0_VA + PGSIZE), idle_tid,
        (void *)IDLE_PGDIR_VA, (void *)idle_pgdir_pa, (void *)IDLE_VS_VA,
        (void *)IDLE_NODE_VA, (void *)IDLE_VADDR_VA, null, TIMETICKS, null);

    // setup idle page directory table
    pgelem_t flag = (pgelem_t)PGENT_US | PGENT_RW | PGENT_PS;
    uint32_t i = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING);
    for (; i < (PGSIZE / sizeof(uint32_t) - 1); ++i)
        ((pgelem_t *)idle_pgdir_va)[i] = ((pgelem_t *)hoo_pcb->pdir_va_)[i];
    ((pgelem_t *)idle_pgdir_va)[0] = (pgelem_t)idle_pg_pa | flag;
    ((pgelem_t *)idle_pgdir_va)[i] = (pgelem_t)idle_pgdir_pa | flag;

    // setup idle page table
    ((pgelem_t *)hoo_temp_pg_va)[0] = (pgelem_t)idle_ring0_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[1] = (pgelem_t)idle_ring3_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[2] = (pgelem_t)idle_vspace_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[3] = (pgelem_t)idle_node_pa | flag;
    ((pgelem_t *)hoo_temp_pg_va)[4] = (pgelem_t)idle_vaddr_pa | flag;

    pcb_t *idle_pcb = pcb_get(idle_tid);
    node_t *n = mdata_alloc_node(idle_tid);
    node_set(n, idle_pcb, null);
    task_ready(n);

    // release
    vir_release_pages(hoo_pcb, va);
}

/**
 * @brief setup the virtual space of the idle thread
 */
void
idle_setup_vspace(void) {
    vir_alloc_pages(pcb_get(TID_IDLE), IDLE_PAGES);
}

/**
 * @brief copy idle thread
 * 
 * @return thread id of the new thread
 */
tid_t
fork(void) {
    pcb_t *hoo_pcb = get_hoo_pcb();
    void *new_pgdir_pa = phy_alloc_page();
    void *hoo_pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(hoo_pcb->pdir_va_, (uint32_t)hoo_pgdir_va,
        (uint32_t)new_pgdir_pa);

    // copy the page directory table
    uint32_t i = 0;
    for (; i < (PGSIZE / sizeof(uint32_t) - 1); ++i)
        ((pgelem_t *)hoo_pgdir_va)[i] = ((pgelem_t *)idle_pgdir_va)[i];
    ((pgelem_t *)hoo_pgdir_va)[0] &= ~((pgelem_t)PGENT_RW);
    ((pgelem_t *)hoo_pgdir_va)[i] =
        (pgelem_t)new_pgdir_pa | PGENT_US | PGENT_RW | PGENT_PS;

    // copy pcb
    tid_t new_tid = allocate_tid();
    pcb_t *new_pcb = pcb_get(new_tid);
    pcb_t *idle_pcb = pcb_get(TID_IDLE);
    pcb_set(idle_pcb->stack_cur_, idle_pcb->stack0_, new_tid, idle_pcb->pdir_va_,
        new_pgdir_pa, idle_pcb->vmngr_.vspace_, idle_pcb->vmngr_.node_,
        idle_pcb->vmngr_.vaddr_, &idle_pcb->vmngr_.head_, TIMETICKS,
        idle_pcb->sleeplock_);

    // add to ready queue
    node_t *n = mdata_alloc_node(new_tid);
    node_set(n, new_pcb, null);
    task_ready(n);

    return new_tid;
}

/**
 * @brief make the task to sleep (the sleeplock MUST BE held to make sure
 * only one task in scheduling each time)
 * 
 * @param slock sleeplock
 */
void
sleep(sleeplock_t *slock) {
    if (slock == null)    panic("sleep(): null pointer");
    if (test(&slock->guard_) == 0)    panic("sleep(): invalid sleeplock");

    // it is time to go to sleep
    disable_intr();
    pcb_t *cur = get_current_pcb();
    cur->sleeplock_ = slock;
    signal(&slock->guard_);
    scheduler();

    // it is time to wakeup
    enable_intr();
    cur->sleeplock_ = null;

    wait(&slock->guard_);
}

/**
 * @brief wakeup the task (the sleeplock MUST BE held to make sure
 * only one task in scheduling each time)
 * 
 * @param slock sleeplock
 */
void
wakeup(sleeplock_t *slock) {
    if (test(&slock->guard_) == 0)    panic("sleep(): invalid sleeplock");

    for (uint32_t i = 1; i <= __list_sleeping.size_; ++i) {
        node_t *n = list_find(&__list_sleeping, i);
        if (((pcb_t *)n->data_)->sleeplock_ == slock) {
            node_t *to_wakeup = list_remove(&__list_sleeping, i);
            task_ready(to_wakeup);
            --i;
        }
    }
}

/**
 * @brief hold the sleeplock
 * 
 * @param slock sleeplock
 */
void
wait_sleeplock(sleeplock_t *slock) {
    if (slock == null)    panic("wait_sleeplock(): null pointer");

    wait(&slock->guard_);
    while (slock->islock_)    sleep(slock);

    // nobody holds the sleeplock
    get_current_pcb()->sleeplock_ = slock;
    slock->islock_ = 1;
    signal(&slock->guard_);
}

/**
 * @brief release the sleeplock
 * 
 * @param slock sleeplock
 */
void
signal_sleeplock(sleeplock_t *slock) {
    if (slock == null)    panic("signal_sleeplock(): null pointer");

    wait(&slock->guard_);
    slock->islock_ = 0;
    get_current_pcb()->sleeplock_ = null;
    wakeup(slock);
    signal(&slock->guard_);
}
