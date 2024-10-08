/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

__attribute__((aligned(4096))) static uint32_t mdata_vspace[PG_STRUCT_SIZE] = { 0 },
    mdata_node[PG_STRUCT_SIZE] = { 0 }, mdata_vaddr[PG_STRUCT_SIZE] = { 0 };
__attribute__((aligned(4096))) static char current_dir[PGSIZE] = { 0 };

/**
 * @brief bucket size array
 */
static uint32_t __bucket_size[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };
#define MAX_BUCKET_SIZE     (NELEMS(__bucket_size))

/**
 * @brief the bucket manager one thread owned
 */
typedef struct arr_buckmngr {
    buckx_mngr_t head_[MAX_BUCKET_SIZE];
} thread_buckmngr_t;

// serially access task queues
static spinlock_t __spinlock_tasks;
// serially access metadata
static spinlock_t __spinlock_mdata_node;
// serially access pcb array
static spinlock_t __spinlock_pcb;
// serially access thread id
static spinlock_t __spinlock_alloc_tid;
static queue_t __queue_ready, __queue_running;
static list_t __list_sleeping, __list_expired;
static node_t *__mdata_node;
static pcb_t *__mdata_pcb;
static thread_buckmngr_t *__mdata_buckmngr;
static uint8_t *__mdata_bmbuff_tid;
static bitmap_t __bm_tid;

#ifdef DEBUG

/**
 * @brief debug mode: print queue
 * @param queue the queue to print
 */
static void
debug_print_queue(queue_t *queue) {
    if (queue == null)    return;
    for (node_t *n = queue->head_->next_; n; n = n->next_) {
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }
}

/**
 * @brief debug mode: print list
 * @param list the list to print
 */
static void
debug_print_list(list_t *list) {
    if (list == null)    return;
    for (idx_t i = 1; i <= list->size_; ++i) {
        node_t *n = list_find(list, i);
        kprintf("0x%x", n);
        if (n->next_)    kprintf(", ");
    }
}

/**
 * @brief debug mode: print tasks metadata
 */
void
debug_print_tasks() {
    kprintf(
        "[DEBUG] sizeof(vsmngr_t): %d\tsizeof(pcb_t): %d\n"
        "[DEBUG] hoo pcb pointer:    0x%x\n"
        "[DEBUG] __mdata_node:       0x%x\n"
        "[DEBUG] __mdata_pcb:        0x%x\n"
        "[DEBUG] __queue_ready:      0x%x\n"
        "[DEBUG] __queue_running:    0x%x\n"
        "[DEBUG] __list_sleeping:    0x%x\n"
        "[DEBUG] __list_expired:     0x%x\n",
        sizeof(vsmngr_t), sizeof(pcb_t), get_hoo_pcb(),
        __mdata_node, __mdata_pcb, &__queue_ready, &__queue_running,
        &__list_sleeping, &__list_expired);
    debug_print_pcb(get_hoo_pcb());

    kprintf("\n[DEBUG] running: ");
    debug_print_queue(&__queue_running);

    kprintf("\n[DEBUG] ready: ");
    debug_print_queue(&__queue_ready);

    kprintf("\n[DEBUG] sleeping: ");
    debug_print_list(&__list_sleeping);

    kprintf("\n[DEBUG] expired: ");
    debug_print_list(&__list_expired);

    kprintf("\n");
}

/**
 * @brief debug mode: print virtual space of current pcb
 */
void
debug_print_vspace(void) {
    debug_print_vspace_pcb(get_current_pcb());
}

void
debug_print_pgdir(void) {
    kprintf("\n[DEBUG] page directories: \n");
    for (uint32_t i = 0; i < PG_STRUCT_SIZE; ++i) {
        pgelem_t *pde = (pgelem_t *)GET_PDE(i * MB4);
        pgelem_t pgtbl_addr = (pgelem_t)PG(*pde);
        if (pgtbl_addr) {
            kprintf("PDE[%d] 0x%x\n\t", i, pgtbl_addr);
            for (uint32_t j = 0; j < PG_STRUCT_SIZE; ++j) {
                pgelem_t *pte = (pgelem_t *)GET_PTE(0 | (i << 22) | (j << 12));
                pgelem_t addr = (pgelem_t)PG(*pte);
                if (addr)    kprintf("%d, ", j);
            } // end for(j)
            kprintf("\n");
        }
    } // end for(i)
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
 * @brief get the bucket manager metadata
 * 
 * @param tid thread id
 * @return bucket manager metadata
 */
static buckx_mngr_t *
thread_buckmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_buckmngr[tid].head_;
}

/**
 * @brief bucket manager initialization of single thread
 * 
 * @param tb thread bucket manager
 */
static void
thread_buckmngr_set(thread_buckmngr_t *tb) {
    if (tb == null)    panic("thread_buckmngr_set(): null pointer");

    buckx_mngr_t *worker = tb->head_;
    for (int i = 0; i < MAX_BUCKET_SIZE; ++i) {
        buckx_mngr_t *next = (i == MAX_BUCKET_SIZE - 1) ? null : worker + i + 1;
        buckmngr_init(worker + i, __bucket_size[i], null, next);
    }
}

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    list_init(&__list_sleeping, false);
    list_init(&__list_expired, false);
    spinlock_init(&__spinlock_tasks);
    spinlock_init(&__spinlock_mdata_node);
    spinlock_init(&__spinlock_pcb);
    spinlock_init(&__spinlock_alloc_tid);

    // metadata
    bzero(mdata_vspace, sizeof(mdata_vspace));
    bzero(mdata_node, sizeof(mdata_node));
    bzero(mdata_vaddr, sizeof(mdata_vaddr));
    bzero(current_dir, sizeof(current_dir));

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_t *hoo_pcb = get_hoo_pcb();

    static thread_buckmngr_t hoo_bucket;
    thread_buckmngr_set(&hoo_bucket);
    fmngr_t hoo_fmngr;
    pcb_set(hoo_pcb, (uint32_t *)STACK_HOO_RING0, (uint32_t *)STACK_HOO_RING3,
        TID_HOO, (pgelem_t *)(V2P(get_hoo_pgdir())), mdata_vspace, mdata_node,
        mdata_vaddr, null, TIMETICKS, null, hoo_bucket.head_, &hoo_fmngr, 0,
        INVALID_INDEX, current_dir, sizeof(current_dir));

    static node_t hoo_node;
    node_set(&hoo_node, hoo_pcb, null);
    wait(&__spinlock_tasks);
    queue_push(&__queue_running, &hoo_node, TAIL);
    signal(&__spinlock_tasks);

    vir_alloc_pages(hoo_pcb, (KERN_HIGH_MAPPING + MB4) / PGSIZE);
    task_init_fmngr(&hoo_pcb->fmngr_);
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;

    // initialize the tasks system
    uint32_t metadata_node_pages =
        (sizeof(node_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_node = vir_alloc_pages(hoo_pcb, metadata_node_pages);

    for (uint32_t i = 0; i < metadata_node_pages; ++i) {
        void *va = (void *)((uint32_t)__mdata_node + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }

    uint32_t metadata_pcb_pages =
        (sizeof(pcb_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_pcb = vir_alloc_pages(hoo_pcb, metadata_pcb_pages);
    for (uint32_t i = 0; i < metadata_pcb_pages; ++i) {
        void *va = (void *)((uint32_t)__mdata_pcb + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }

    uint32_t metadata_bucket_pages =
        (sizeof(thread_buckmngr_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_buckmngr = vir_alloc_pages(hoo_pcb, metadata_bucket_pages);
    for (uint32_t i = 0; i < metadata_bucket_pages; ++i) {
        void * va = (void *)((uint32_t)__mdata_buckmngr + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }
    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i)
        thread_buckmngr_set(__mdata_buckmngr + i);

    uint32_t metadata_tid_pages =
        (sizeof(uint8_t) * (MAX_TASKS_AMOUNT / BITS_PER_BYTE) + PGSIZE - 1) / PGSIZE;
    __mdata_bmbuff_tid = vir_alloc_pages(hoo_pcb, metadata_tid_pages);
    for (uint32_t i = 0; i < metadata_tid_pages; ++i) {
        void *va = (void *)((uint32_t)__mdata_bmbuff_tid + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }
    bzero(__mdata_bmbuff_tid, sizeof(__mdata_bmbuff_tid));
    bitmap_init(&__bm_tid, MAX_TASKS_AMOUNT, __mdata_bmbuff_tid);
    // for hoo thread
    bitmap_set(&__bm_tid, TID_HOO);
    // for idle thread
    bitmap_set(&__bm_tid, TID_IDLE);
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
    if (cur && !((pcb_t *)cur->data_)->sleeplock_ ) {
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
        tss->esp0_ = PGUP(((pcb_t *)next->data_)->stack0_, PGSIZE);

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
 * @brief setup the ring0 stack
 * 
 * @param r0    top of ring 0 stack
 * @param r3    top of ring 3 stack
 * @param entry thread entry
 * @return ring0 stack pointer after setup
 */
static uint32_t *
setup_ring0_stack(void *r0, void *r3, void *entry) {

    // setup user stack
    uint8_t *pstack = (uint8_t *)r3;
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
    *((uint32_t *)pstack) = (uint32_t)r3 - sizeof(uint32_t);

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
 * @brief get pcb
 * 
 * @param tid thread id
 * @return pcb pointer
 */
static pcb_t *
pcb_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_get(): thread id overflow");

    pcb_t *temp = null;
    wait(&__spinlock_pcb);
    temp = __mdata_pcb + tid;
    signal(&__spinlock_pcb);
    return temp;
}

/**
 * @brief setup the first ring3 thread, idle
 * @note MUST BE setup by kernel,
 * because this logic will use the kernel linear space
 * @param entry the entry point
 */
void
idle_init(void *entry) {
    pcb_t *hoo_pcb = get_hoo_pcb();
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
    uint32_t kern_beg = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING),
        kern_end = PG_STRUCT_SIZE - 1;

    // setup the idle page directory table (1 page)
    void *pgdir_pa = phy_alloc_page();
    void *pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(pgdir_va, pgdir_pa, flags);
    for (uint32_t i = kern_beg; i < kern_end; ++i)
        ((pgelem_t *)pgdir_va)[i] = *((pgelem_t *)PG_DIR_VA + i);
    ((pgelem_t *)pgdir_va)[kern_end] = (pgelem_t)pgdir_pa | flags;

    // setup the idle ring0 stack (1 page)
    void *ring0_pa = phy_alloc_page();
    void *ring0_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(ring0_va, ring0_pa, flags);

    // setup the idle ring3 stack (1 page)
    void *ring3_pa = phy_alloc_page();
    void *ring3_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(ring3_va, ring3_pa, flags);

    // setup metadata (3 pages)
    void *mdata_vspace_pa = phy_alloc_page();
    void *mdata_vspace_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(mdata_vspace_va, mdata_vspace_pa, flags);
    void *mdata_node_pa = phy_alloc_page();
    void *mdata_node_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(mdata_node_va, mdata_node_pa, flags);
    void *mdata_vaddr_pa = phy_alloc_page();
    void *mdata_vaddr_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(mdata_vaddr_va, mdata_vaddr_pa, flags);

    // setup the current directory
    void *dir_pa = phy_alloc_page();
    char *dir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(dir_va, dir_pa, flags);

    // setup the idle thread
    uint32_t *cur_stack = setup_ring0_stack(ring0_va + PGSIZE,
        ring3_va + PGSIZE, entry);
    pcb_t *idle_pcb = pcb_get(TID_IDLE);
    fmngr_t idle_fmngr;
    task_init_fmngr(&idle_fmngr);
    pcb_set(idle_pcb, cur_stack, ring3_va + PGSIZE, TID_IDLE, pgdir_pa,
        mdata_vspace_va, mdata_node_va, mdata_vaddr_va, null, TIMETICKS,
        null, thread_buckmngr_get(TID_IDLE), &idle_fmngr, VIR_BASE_IDLE,
        INVALID_INDEX, dir_va, PGSIZE);

    node_t *n = mdata_alloc_node(TID_IDLE);
    node_set(n, idle_pcb, null);
    task_ready(n);

}

/**
 * @brief allocate the thread id
 * 
 * @return thread id
 */
static tid_t
allocate_tid() {
    tid_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = (tid_t)bitmap_scan_empty(&__bm_tid);
    signal(&__spinlock_alloc_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("allocate_tid(): thread id overflows");
    return temp;
}

/**
 * @brief copy idle thread
 * @param entry the entry of the new process
 * @return thread id of the new thread in the parent; 0 in the child
 */
tid_t
fork(void *entry) {

    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
    pcb_t *hoo_pcb = get_hoo_pcb(), *cur_pcb = get_current_pcb();

    // page directory table
    void *new_pgdir_pa = phy_alloc_page();
    pgelem_t *new_pgdir_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_pgdir_va, new_pgdir_pa, flags);

    // ring0
    void *new_ring0_pa = phy_alloc_page();
    void *new_ring0_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_ring0_va, new_ring0_pa, flags);

    // ring3 stack
    void *new_ring3_pa = phy_alloc_page();
    void *new_ring3_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_ring3_va, new_ring3_pa, flags);

    // metadata
    void *new_vspace_pa = phy_alloc_page();
    void *new_vspace_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_vspace_va, new_vspace_pa, flags);
    void *new_node_pa = phy_alloc_page();
    void *new_node_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_node_va, new_node_pa, flags);
    void *new_vaddr_pa = phy_alloc_page();
    void *new_vaddr_va = vir_alloc_pages(hoo_pcb, 1);
    set_mapping(new_vaddr_va, new_vaddr_pa, flags);

    // copy all the linear space
    uint32_t copy_beg = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING),
        copy_end = PG_STRUCT_SIZE - 1;
    for (idx_t i = 0; i < copy_beg; ++i) {
        pgelem_t *pde = (pgelem_t *)PG_DIR_VA + i;
        if (*pde) {
            void *new_page_table = phy_alloc_page();
            pgelem_t *new_page_table_va = vir_alloc_pages(hoo_pcb, 1);
            set_mapping(new_page_table_va, (void *)new_page_table, flags);

            // change all the PTE flags in this PDE of the child to read-only
            for (idx_t j = 0; j < PG_STRUCT_SIZE; ++j) {
                pgelem_t *pte = (pgelem_t *)(
                    ((uint32_t)pde << 10) | (j * sizeof(uint32_t)));
                if (*pte)    new_page_table_va[j] = *pte & ~((pgelem_t)PGENT_RW);
            }

            new_pgdir_va[i] = (pgelem_t)new_page_table | flags;
            vir_release_pages(hoo_pcb, (void *)new_page_table_va, false);
        } else    new_pgdir_va[i] = 0;
    }
    for (uint32_t i = copy_beg; i < copy_end; ++i)
        new_pgdir_va[i] = *((pgelem_t *)PG_DIR_VA + i);
    new_pgdir_va[copy_end] = (pgelem_t)new_pgdir_pa | flags;

    // copy pcb
    tid_t new_tid = allocate_tid();
    pcb_t *new_pcb = pcb_get(new_tid);
    uint32_t *cur_stack = setup_ring0_stack(new_ring0_va + PGSIZE,
        new_ring3_va + PGSIZE, entry);
    pcb_set(new_pcb, cur_stack, new_ring3_va + PGSIZE, new_tid, new_pgdir_pa,
        new_vspace_va, new_node_va, new_vaddr_va, &cur_pcb->vmngr_.head_,
        TIMETICKS, null, thread_buckmngr_get(new_tid), &cur_pcb->fmngr_,
        VIR_BASE_IDLE, cur_pcb->tid_, cur_pcb->dir_, cur_pcb->dirlen_);

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
    __asm__ ("pusha");
    scheduler();
    __asm__ ("popa");

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
 * @brief sleeping to wait the child exiting
 * 
 * @param slock sleeplock resource
 */
void
wait_child(sleeplock_t *slock) {
    if (slock == null)    panic("wait_child(): null pointer");
    wait(&slock->guard_);
    sleep(slock);
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

    // release page tables
    for (idx_t i = 0; i < PD_INDEX(KERN_HIGH_MAPPING); ++i) {
        pgelem_t *pde = (pgelem_t *)(PG_DIR_VA + i * sizeof(uint32_t));
        if (*pde & ~PG_MASK) {
            for (idx_t j = 0; j < PG_STRUCT_SIZE; ++j) {
                pgelem_t *pte = (pgelem_t *)(
                    ((uint32_t)pde << 10) | (j * sizeof(uint32_t)));
                if (*pte & ~PG_MASK) {
                    phy_release_page((void *)(*pte & PG_MASK));
                    *pte = 0;
                }
            } // end for(j)

            phy_release_page((void *)(*pde & PG_MASK));
            *pde = 0;
        }
    } // end for(i)

    // wakeup the asleep process
    if (pcb->parent_ != INVALID_INDEX) {
        pcb_t *parent_pcb = pcb_get(pcb->parent_);
        if (parent_pcb->sleeplock_) {
            wait(&parent_pcb->sleeplock_->guard_);
            wakeup(parent_pcb->sleeplock_);
            signal(&parent_pcb->sleeplock_->guard_);
        }
    }

    // append to the expired list
    wait(&__spinlock_tasks);
    node_t *n = queue_pop(&__queue_running);
    list_insert(&__list_expired, n, LSIDX_AFTAIL(&__list_expired));
    signal(&__spinlock_tasks);

    // the "exited" task cannot return to ring3
    scheduler();
}

/**
 * @brief file manager initialization
 * 
 * @param fmngr file manager
 */
void
task_init_fmngr(fmngr_t *fmngr) {
    if (fmngr == null)    panic("task_init_fmngr(): null pointer");
    bzero(fmngr, sizeof(fmngr_t));

    pcb_t *hoo_pcb = get_hoo_pcb();
    uint32_t bitmap_pages = (MAX_FILES_PER_TASK / BITS_PER_BYTE + PGSIZE) / PGSIZE;
    void *va = vir_alloc_pages(hoo_pcb, bitmap_pages);
    for (uint32_t i = 0; i < bitmap_pages; ++i) {
        void *pa = phy_alloc_page();
        // we could revise the kernel mappings anytime as long as
        //   using the kernel linear addresses
        set_mapping((void *)((uint32_t)va + i * PGSIZE), pa,
            (pgelem_t)PGENT_US | PGENT_RW | PGENT_PS);
    }
    bitmap_init(&fmngr->fd_set_, MAX_FILES_PER_TASK, va);

    // for stdin, stdout, stderr
    bitmap_set(&fmngr->fd_set_, FD_STDIN);
    bitmap_set(&fmngr->fd_set_, FD_STDOUT);
    bitmap_set(&fmngr->fd_set_, FD_STDERR);

    uint32_t file_arr_pages = (MAX_FILES_PER_TASK * sizeof(fd_t) + PGSIZE) / PGSIZE;
    va = vir_alloc_pages(hoo_pcb, file_arr_pages);
    for (uint32_t i = 0; i < file_arr_pages; ++i) {
        void *pa = phy_alloc_page();
        // we could revise the kernel mappings anytime as long as
        //   using the kernel linear addresses
        set_mapping((void *)((uint32_t)va + i * PGSIZE), pa,
            (pgelem_t)PGENT_US | PGENT_RW | PGENT_PS);
    }
    fmngr->files_ = va;
}

/**
 * @brief clear resources of the expired task
 */
void
kill(void) {

    for (idx_t i = 1; i <= __list_expired.size_; ++i) {
        node_t *n = list_find(&__list_expired, i);
        pcb_t *pcb = (pcb_t *)n->data_;
        list_remove(&__list_expired, i);

        // release ring0 stack
        phy_release_vpage(pcb, (void *)PGDOWN(pcb->stack0_, PGSIZE));

        // release ring3 stack
        phy_release_vpage(pcb,
            (void *)PGDOWN(((void *)pcb->stack3_ - PGSIZE), PGSIZE));

        // release the working directories if it is the parent
        if (pcb->parent_ == INVALID_INDEX)
            phy_release_vpage(pcb, pcb->dir_);

        // release page directory table
        phy_release_page(pcb->pgdir_pa_);
        bzero(pcb, sizeof(pcb_t));

    } // end for()
}
