/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

static node_t __mdata_node[MAX_TASKS_AMOUNT];
// serially access task queues
static spinlock_t __spinlock_tasks;
// serially access metadata
static spinlock_t __spinlock_mdata_node;
static queue_t __queue_ready, __queue_running;
static list_t __list_sleeping, __list_expired;
static const uint32_t IDLE_PAGES = 4;
static const pgelem_t IDLE_RING3_VA = 0;
static const pgelem_t IDLE_MD_VSPACE_VA = IDLE_RING3_VA + PGSIZE;
static const pgelem_t IDLE_MD_NODE_VA = IDLE_MD_VSPACE_VA + PGSIZE;
static const pgelem_t IDLE_MD_VADDR_VA = IDLE_MD_NODE_VA + PGSIZE;

#ifdef DEBUG
/**
 * @brief debug mode: print tasks metadata
 */
void
debug_print_tasks() {
    kprintf(
        "[DEBUG] sizeof(pgstruct_t): %d\tsizeof(vsmngr_t): %d\tsizeof(pcb_t): %d\n"
        "[DEBUG] pcb_slot:           0x%x\n"
        "[DEBUG] __mdata_node:       0x%x\n"
        "[DEBUG] __queue_ready:      0x%x\n"
        "[DEBUG] __queue_running:    0x%x\n"
        "[DEBUG] __list_sleeping:    0x%x\n"
        "[DEBUG] __list_expired:     0x%x\n",
        sizeof(pgstruct_t), sizeof(vsmngr_t), sizeof(pcb_t),
        pcb_get(TID_HOO), __mdata_node, &__queue_ready, &__queue_running,
        &__list_sleeping, &__list_expired);

    kprintf("\n[DEBUG] running: ");
    for (node_t *n = __queue_running.head_->next_; n; n = n->next_) {
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }

    kprintf("\n[DEBUG] ready: ");
    for (node_t *n = __queue_ready.head_->next_; n; n = n->next_) {
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }

    kprintf("\n[DEBUG] sleeping: ");
    for (idx_t i = 1; i <= __list_sleeping.size_; ++i) {
        node_t *n = list_find(&__list_sleeping, i);
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }

    kprintf("\n[DEBUG] expired: ");
    for (idx_t i = 1; i <= __list_expired.size_; ++i) {
        node_t *n = list_find(&__list_expired, i);
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }

    kprintf("\n");
}
#endif

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    wait(&__spinlock_tasks);
    node_t *cur = queue_front(&__queue_running);
    signal(&__spinlock_tasks);
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
    list_init(&__list_expired, false);
    spinlock_init(&__spinlock_tasks);
    spinlock_init(&__spinlock_mdata_node);

    // metadata
    __attribute__((aligned(4096))) static pgelem_t mappings[PG_STRUCT_SIZE] = { 0 };
    __attribute__((aligned(4096))) static uint32_t
        mdata_vspace[PG_STRUCT_SIZE] = { 0 }, mdata_node[PG_STRUCT_SIZE] = { 0 },
        mdata_vaddr[PG_STRUCT_SIZE] = { 0 };
    bzero(mappings, sizeof(mappings));
    bzero(mdata_vspace, sizeof(mdata_vspace));
    bzero(mdata_node, sizeof(mdata_node));
    bzero(mdata_vaddr, sizeof(mdata_vaddr));

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_t *hoo_pcb = get_hoo_pcb();

    pgstruct_t pgs;
    pgstruct_set(&pgs, get_hoo_pgdir(), (void *)(V2P(get_hoo_pgdir())), mappings);
    pcb_set(null, (uint32_t *)STACK_HOO_RING0, hoo_pcb->tid_, &pgs,
        mdata_vspace, mdata_node, mdata_vaddr, null, TIMETICKS, null);
    node_t *n = mdata_alloc_node(hoo_pcb->tid_);
    node_set(n, hoo_pcb, null);
    wait(&__spinlock_tasks);
    queue_push(&__queue_running, n, TAIL);
    signal(&__spinlock_tasks);

    vir_alloc_pages(hoo_pcb, (KERN_HIGH_MAPPING + MM_BASE) / PGSIZE);

    // fill up kernel mappings
    uint32_t beg = PD_INDEX(KERN_HIGH_MAPPING) + 1, end = (PG_STRUCT_SIZE - 1);
    mappings[0] = (uint32_t)KERN_HIGH_MAPPING + SEG_PGTABLE * 16;
    mappings[beg - 1] = mappings[0];
    mappings[end] = (uint32_t)hoo_pcb->pgstruct_.pdir_va_;
    void *va = vir_alloc_pages(hoo_pcb, end - beg), *pa = null;
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
    for (uint32_t i = beg; i < end; ++i) {
        pa = phy_alloc_page();
        uint32_t va32 = (uint32_t)va + (i - beg) * PGSIZE;
        mappings[i] = va32;
        *((pgelem_t *)hoo_pcb->pgstruct_.pdir_va_ + i) = (pgelem_t)pa | flags;
        set_mapping(&hoo_pcb->pgstruct_, va32, (uint32_t)pa, flags);
    }
}

/**
 * @brief scheduler two thread
 */
void
scheduler() {
    if (test(&__spinlock_tasks))    return;

    // to check whether ticks expired
    // we just need the first queue node
    wait(&__spinlock_tasks);
    node_t *cur = queue_front(&__queue_running);
    if (!((pcb_t *)cur->data_)->sleeplock_ && cur) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            signal(&__spinlock_tasks);
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

        signal(&__spinlock_tasks);
        switch_to(cur, next);
    }

    signal(&__spinlock_tasks);
}

/**
 * @brief make a task get ready
 * 
 * @param task the node of a task pcb object
 */
void
task_ready(node_t *task) {
    wait(&__spinlock_tasks);
    queue_push(&__queue_ready, task, TAIL);
    signal(&__spinlock_tasks);
}

/**
 * @brief setup the ring0 stack of hoo
 * 
 * @param r0 top of idle thread ring 0 stack
 * @param r3_idle top of idle thread ring 3 stack (idle access allowing)
 * @param r3_hoo top of idle thread ring 3 stack (hoo access allowing)
 * @param entry  thread entry
 * @return ring0 stack pointer after setup
 */
static uint32_t *
setup_idle_ring0_stack(void *r0, void *r3_idle, void *r3_hoo, void *entry) {

    // setup user stack
    uint8_t *pstack = (uint8_t *)r3_hoo;
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (DIED_INSTRUCTION + KERN_HIGH_MAPPING);

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
    pstack = (uint8_t *)r0;

    // always located on the top of new task stack that the `esp`
    // pointed to when the new task completes its initialization
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (DIED_INSTRUCTION + KERN_HIGH_MAPPING);

    // user mode entry
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)entry;

    // user mode stack
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)r3_idle - sizeof(uint32_t);

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
idle_init(void) {
    pcb_t *hoo_pcb = get_hoo_pcb();
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
    uint32_t kern_beg = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING),
        kern_end = PG_STRUCT_SIZE - 1;

    // setup the idle page directory table (1 page)
    void *idle_pgdir_pa = phy_alloc_page();
    void *idle_pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)idle_pgdir_va,
        (uint32_t)idle_pgdir_pa, flags);
    for (uint32_t i = kern_beg; i < kern_end; ++i)
        ((pgelem_t *)idle_pgdir_va)[i] =
            ((pgelem_t *)hoo_pcb->pgstruct_.pdir_va_)[i];
    ((pgelem_t *)idle_pgdir_va)[kern_end] = (pgelem_t)idle_pgdir_pa | flags;

    // setup the idle page tables mapping (1 page)
    void *idle_mapping_pa = phy_alloc_page();
    pgelem_t *idle_mapping_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)idle_mapping_va,
        (uint32_t)idle_mapping_pa, flags);
    for (uint32_t i = kern_beg; i < kern_end; ++i)
        ((pgelem_t *)idle_mapping_va)[i] = hoo_pcb->pgstruct_.mapping_[i];
    ((pgelem_t *)idle_mapping_va)[kern_end] = 0;

    // setup the idle page table (1 page)
    void *idle_pg_pa = phy_alloc_page();
    void *idle_pg_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)idle_pg_va,
        (uint32_t)idle_pg_pa, flags);
    ((pgelem_t *)idle_pgdir_va)[0] = (pgelem_t)idle_pg_pa | flags;
    ((pgelem_t *)idle_mapping_va)[0] = (pgelem_t)idle_pg_va;

    // setup the idle ring0 stack (1 page)
    void *idle_ring0_pa = phy_alloc_page();
    void *hoo_ring0_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)hoo_ring0_va,
        (uint32_t)idle_ring0_pa, flags);

    // setup the idle ring3 stack (1 page)
    void *idle_ring3_pa = phy_alloc_page();
    void *hoo_ring3_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)hoo_ring3_va,
        (uint32_t)idle_ring3_pa, flags);
    ((pgelem_t *)idle_pg_va)[PT_INDEX(IDLE_RING3_VA)] =
        (pgelem_t)idle_ring3_pa | flags;

    // setup the idle metadata (3 pages)
    void *idle_vspace_pa = phy_alloc_page();
    ((pgelem_t *)idle_pg_va)[PT_INDEX(IDLE_MD_VSPACE_VA)] =
        (pgelem_t)idle_vspace_pa | flags;

    void *idle_node_pa = phy_alloc_page();
    ((pgelem_t *)idle_pg_va)[PT_INDEX(IDLE_MD_NODE_VA)] =
        (pgelem_t)idle_node_pa | flags;

    void *idle_vaddr_pa = phy_alloc_page();
    ((pgelem_t *)idle_pg_va)[PT_INDEX(IDLE_MD_VADDR_VA)] =
        (pgelem_t)idle_vaddr_pa | flags;

    // setup the idle thread
    uint32_t *cur_stack = setup_idle_ring0_stack(
        (void *)((uint32_t)hoo_ring0_va + PGSIZE),
        (void *)(IDLE_RING3_VA + PGSIZE),
        (void *)(hoo_ring3_va + PGSIZE), idle);
    pgstruct_t pgs;
    pgstruct_set(&pgs, (void *)idle_pgdir_va, (void *)idle_pgdir_pa,
        idle_mapping_va);
    pcb_set(cur_stack, (uint32_t *)((uint32_t)hoo_ring0_va + PGSIZE), TID_IDLE,
        &pgs, (void *)IDLE_MD_VSPACE_VA, (void *)IDLE_MD_NODE_VA,
        (void *)IDLE_MD_VADDR_VA, null, TIMETICKS, null);

    pcb_t *idle_pcb = pcb_get(TID_IDLE);
    node_t *n = mdata_alloc_node(TID_IDLE);
    node_set(n, idle_pcb, null);
    task_ready(n);

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
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
    pcb_t *hoo_pcb = get_hoo_pcb(), *idle_pcb = pcb_get(TID_IDLE);
    uint32_t idle_beg = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING),
        idle_end = PG_STRUCT_SIZE - 1;

    // page directory table
    void *new_pgdir_pa = phy_alloc_page();
    void *new_pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)new_pgdir_va,
        (uint32_t)new_pgdir_pa, flags);
    for (uint32_t i = idle_beg; i < idle_end; ++i)
        ((pgelem_t *)new_pgdir_va)[i] =
            ((pgelem_t *)idle_pcb->pgstruct_.pdir_va_)[i];
    ((pgelem_t *)new_pgdir_va)[idle_end] = (pgelem_t)new_pgdir_pa | flags;

    // page table mappings
    void *new_mapping_pa = phy_alloc_page();
    void *new_mapping_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)new_mapping_va,
        (uint32_t)new_mapping_pa, flags);
    for (uint32_t i = idle_beg; i < idle_end; ++i)
        ((pgelem_t *)new_mapping_va)[i] =
            ((pgelem_t *)idle_pcb->pgstruct_.pdir_va_)[i];
    ((pgelem_t *)new_mapping_va)[idle_end] = 0;

    // ring0
    void *new_ring0_pa = phy_alloc_page();
    void *new_ring0_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)new_ring0_va,
        (uint32_t)new_ring0_pa, flags);

    // page table
    void *new_pg_pa = phy_alloc_page();
    void *new_pg_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(&hoo_pcb->pgstruct_, (uint32_t)new_pg_va,
        (uint32_t)new_pg_pa, flags);
    ((pgelem_t *)new_pgdir_va)[0] = (pgelem_t)new_pg_pa | flags;
    ((pgelem_t *)new_mapping_va)[0] = (pgelem_t)new_pg_va;

    // copy the first 4MB of idle linear space
    idx_t idle_pde_4mb = PD_INDEX(MB4) - 1;
    pgelem_t *idle_pg_4mb = idle_pcb->pgstruct_.mapping_ + idle_pde_4mb;
    for (uint32_t i = 0; i < PG_STRUCT_SIZE; ++i)
        ((pgelem_t *)new_pg_va)[i] = idle_pg_4mb[i] & ~PGENT_RW;

    // copy pcb
    tid_t new_tid = allocate_tid();
    pcb_t *new_pcb = pcb_get(new_tid);
    pgstruct_t pgs;
    pgstruct_set(&pgs, new_pgdir_va, new_pgdir_pa, new_mapping_va);
    pcb_set(idle_pcb->stack_cur_, new_ring0_va, new_tid, &pgs,
        idle_pcb->vmngr_.vspace_, idle_pcb->vmngr_.node_, idle_pcb->vmngr_.vaddr_,
        &idle_pcb->vmngr_.head_, TIMETICKS, idle_pcb->sleeplock_);

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
            node_t *to_wakeup = list_remove(&__list_sleeping, i--);
            task_ready(to_wakeup);
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

/**
 * @brief exit the current task
 */
void
exit() {
    pcb_t *pcb = get_current_pcb();

    // release vspace
    release_vspace(pcb);

    // release metadata
    phy_release_vpage(pcb, pcb->vmngr_.vspace_);
    phy_release_vpage(pcb, pcb->vmngr_.node_);
    phy_release_vpage(pcb, pcb->vmngr_.vaddr_);

    // release ring3 stack
    phy_release_vpage(pcb, (void *)IDLE_RING3_VA);

    // append to the expired list
    wait(&__spinlock_tasks);
    node_t *n = queue_pop(&__queue_running);
    list_insert(&__list_expired, n, LSIDX_AFTAIL(&__list_expired));
    signal(&__spinlock_tasks);
}

/**
 * @brief kill the specific task by the current task if in expired list
 * 
 * @param pcb the task going to be terminated
 */
void
kill(pcb_t *pcb) {
    if (pcb == null)    panic("kill(): null pointer");
    bool is_finish_exit = false;
    for (idx_t i = 1; i <= __list_expired.size_; ++i) {
        node_t *n = list_find(&__list_expired, i);
        if (n->data_ == pcb) {
            list_remove(&__list_expired, i);
            is_finish_exit = true;
            break;
        }
    }
    // the conditions still not to meet
    if (!is_finish_exit)    return;


    // release ring0 stack
    pcb_t *master = get_hoo_pcb();
    phy_release_vpage(master, (void *)((uint32_t)pcb->stack0_ - PGSIZE));

    // release page directory table
    phy_release_vpage(master, pcb->pgstruct_.pdir_va_);

    // release page tables
    for (idx_t i = 0; i < PD_INDEX(KERN_HIGH_MAPPING); ++i) {
        void *va = (void *)pcb->pgstruct_.mapping_[i];
        if (va)    phy_release_vpage(master, va);
    }

    // release page tables mapping
    phy_release_vpage(master, pcb->pgstruct_.mapping_);

    bzero(pcb, sizeof(pcb_t));
}
