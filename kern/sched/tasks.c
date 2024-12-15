/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"
#include "pcb.h"
#include "kern/panic.h"
#include "kern/hoo/thread_bucket.h"
#include "kern/hoo/thread_curdir.h"
#include "kern/hoo/thread_fmngr.h"
#include "kern/hoo/thread_pcb.h"
#include "kern/hoo/thread_tid.h"
#include "kern/intr/intr_stack.h"
#include "kern/mem/metadata.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/utilities/list.h"
#include "kern/utilities/queue.h"
#include "kern/utilities/spinlock.h"
#include "user/lib.h"

static queue_t __queue_ready, __queue_running;
static spinlock_t __sl_tasks;
static list_t __list_sleeping, __list_expired;

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
 * @brief make a task get ready
 * 
 * @param task the node of a task pcb object
 */
void
task_ready(node_t *task) {
    wait(&__sl_tasks);
    queue_push(&__queue_ready, task, TAIL);
    signal(&__sl_tasks);
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

    spinlock_init(&__sl_tasks);

    // pcb node append to queue
    node_t *hoo_node = node_alloc();
    node_set(hoo_node, get_hoo_pcb(), null);
    queue_push(&__queue_running, hoo_node, TAIL);
}

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    wait(&__sl_tasks);
    node_t *cur = queue_front(&__queue_running);
    signal(&__sl_tasks);
    if (cur == null)    panic("get_current_pcb(): no current task");

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    if (cur_pcb == null)    panic("get_current_pcb(): current task is null");

    return cur_pcb;
}

/**
 * @brief scheduler two thread
 */
void
scheduler() {
    if (test(&__sl_tasks))    return;

    // to check whether ticks expired
    // we just need the first queue node
    wait(&__sl_tasks);
    node_t *cur = queue_front(&__queue_running);
    if (cur != null && ((pcb_t *)cur->data_)->sleep_ == null) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            signal(&__sl_tasks);
            return;
        } else    ((pcb_t *)cur->data_)->ticks_ = TIMETICKS;
    }

    node_t *next = queue_pop(&__queue_ready);
    if (next != null) {
        cur = queue_pop(&__queue_running);
        // only change tasks when the `cur` task exists
        if (cur != null) {
            if (((pcb_t *)cur->data_)->sleep_ != null)
                list_insert(&__list_sleeping, cur, LSIDX_AFTAIL(&__list_sleeping));
            else    queue_push(&__queue_ready, cur, TAIL);
        }
        queue_push(&__queue_running, next, TAIL);

        // update tss
        tss_t *tss = get_hoo_tss();
        tss->ss0_ = DS_SELECTOR_KERN;
        tss->esp0_ = PGUP(((pcb_t *)next->data_)->stack0_, PGSIZE);

        signal(&__sl_tasks);
        // `switch_to()` CANNOT recogize the case that current tasks is not exist
        //     because `cur` is null which is not 0
        if (cur == null)    cur = 0;
        switch_to(cur, next);
    }

    signal(&__sl_tasks);
}

/**
 * @brief sleep for the advent of the resource, with the corresponding lock
 * releasing
 * 
 * @param resource      the resource to wait
 * @param resource_lock the corresponding spin lock
 */
void
sleep(void *resource, spinlock_t *resource_lock) {
    if (sleep == null)    panic("sleep(): null pointer");
    if (test(resource_lock) == 0)    panic("sleep(): invalid spinlock");

    // it is time to go to sleep
    disable_intr();
    pcb_t *cur = get_current_pcb();
    cur->sleep_ = resource;
    signal(resource_lock);
    __asm__ ("pusha");
    scheduler();
    __asm__ ("popa");

    // it is time to wakeup
    enable_intr();
    cur->sleep_ = null;

    wait(resource_lock);
}

/**
 * @brief wakeup the task sleeping at the specific resource
 * 
 * @param resource the resource added previously
 */
void
wakeup(void *resource) {

    for (uint32_t i = 1; i <= __list_sleeping.size_; ++i) {
        node_t *n = list_find(&__list_sleeping, i);
        if (((pcb_t *)n->data_)->sleep_ == resource) {
            node_t *to_wakeup = list_remove(&__list_sleeping, i--);
            task_ready(to_wakeup);
        }
    }
}

/**
 * @brief copy idle thread
 * @param entry the entry of the new process
 * @return thread id of the new thread in the parent; 0 in the child
 */
tid_t
fork(void *entry) {

    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;
    pcb_t *hoo_pcb = get_hoo_pcb();

    // page directory table
    void *new_pgdir_pa = phy_alloc_page();
    pgelem_t *new_pgdir_va = vir_alloc_pages(&hoo_pcb->vmngr_, 1, hoo_pcb->break_);
    set_mapping(new_pgdir_va, new_pgdir_pa, flags);

    // ring0
    void *new_ring0_pa = phy_alloc_page();
    void *new_ring0_va = vir_alloc_pages(&hoo_pcb->vmngr_, 1, hoo_pcb->break_);
    set_mapping(new_ring0_va, new_ring0_pa, flags);

    // ring3 stack
    void *new_ring3_pa = phy_alloc_page();
    void *new_ring3_va = vir_alloc_pages(&hoo_pcb->vmngr_, 1, hoo_pcb->break_);
    set_mapping(new_ring3_va, new_ring3_pa, flags);

    // copy all the linear space
    uint32_t copy_beg = (uint32_t)PD_INDEX(KERN_HIGH_MAPPING),
        copy_end = PG_STRUCT_SIZE - 1;
    for (int i = 0; i < copy_beg; ++i) {
        pgelem_t *pde = (pgelem_t *)PG_DIR_VA + i;
        if (*pde) {
            void *new_page_table = phy_alloc_page();
            pgelem_t *new_page_table_va =
                vir_alloc_pages(&hoo_pcb->vmngr_, 1, hoo_pcb->break_);
            set_mapping(new_page_table_va, new_page_table, flags);

            // change all the PTE flags in this PDE of the child to read-only
            for (int j = 0; j < PG_STRUCT_SIZE; ++j) {
                pgelem_t *pte = (pgelem_t *)(
                    ((uint32_t)pde << 10) | (j * sizeof(uint32_t)));
                if (*pte)    new_page_table_va[j] = *pte & ~((pgelem_t)PGFLAG_RW);
            }

            new_pgdir_va[i] = (pgelem_t)new_page_table | flags;
            vir_release_pages(&hoo_pcb->vmngr_, (void *)new_page_table_va, false);
        } else    new_pgdir_va[i] = 0;
    }
    for (uint32_t i = copy_beg; i < copy_end; ++i)
        new_pgdir_va[i] = *((pgelem_t *)PG_DIR_VA + i);
    new_pgdir_va[copy_end] = (pgelem_t)new_pgdir_pa | flags;

    // copy pcb
    tid_t new_tid = thread_tid_alloc();
    pcb_t *new_pcb = thread_pcb_get(new_tid);
    uint32_t *cur_stack = setup_ring0_stack(new_ring0_va + PGSIZE,
        new_ring3_va + PGSIZE, entry);
    pcb_t *cur_pcb = get_current_pcb();
    pcb_set(new_pcb, cur_stack, new_ring3_va + PGSIZE, new_tid, new_pgdir_pa,
        &cur_pcb->vmngr_, TIMETICKS, null, thread_buckmngr_get(new_tid),
        thread_fmngr_get(new_tid), cur_pcb->break_, cur_pcb->tid_,
        thread_curdir_get(new_tid));

    // add to ready queue
    node_t *n = node_alloc();
    node_set(n, new_pcb, null);
    task_ready(n);

    return new_tid;
}

/**
 * @brief waiting the child thread terminating
 * 
 * @param sl sleeplock
 */
void
wait_child(sleeplock_t *sl) {
    if (sl == null)    panic("wait_child(): null pointer");
    wait(&sl->guard_);
    sleep(sl, &sl->guard_);
    signal(&sl->guard_);
}

/**
 * @brief exit the current task
 */
void
exit() {
    pcb_t *pcb = get_current_pcb();

    // release metadata
    thread_buckmngr_clear(pcb->tid_);
    thread_curdir_clear(pcb->tid_);
    thread_fmngr_clear(pcb->tid_);
    thread_tid_release(pcb->tid_);

    // release vspace
    release_vspace(pcb);

    // release page tables
    for (int i = 0; i < PD_INDEX(KERN_HIGH_MAPPING); ++i) {
        pgelem_t *pde = (pgelem_t *)(PG_DIR_VA + i * sizeof(uint32_t));
        if (*pde & ~PG_MASK) {
            for (int j = 0; j < PG_STRUCT_SIZE; ++j) {
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
        pcb_t *parent_pcb = thread_pcb_get(pcb->parent_);
        if (parent_pcb->sleep_) {
            wakeup(parent_pcb->sleep_);
        }
    }

    // append to the expired list
    wait(&__sl_tasks);
    node_t *n = queue_pop(&__queue_running);
    list_insert(&__list_expired, n, LSIDX_AFTAIL(&__list_expired));
    signal(&__sl_tasks);

    // the "exited" task cannot return to ring3
    scheduler();
}

/**
 * @brief clear resources of the expired task
 */
void
kill(void) {

    for (int i = 1; i <= __list_expired.size_; ++i) {
        node_t *n = list_find(&__list_expired, i);
        pcb_t *pcb = (pcb_t *)n->data_;
        list_remove(&__list_expired, i);

        // release ring0 stack
        phy_release_vpage((void *)PGDOWN(pcb->stack0_, PGSIZE));

        // release ring3 stack
        phy_release_vpage((void *)PGDOWN(((void *)pcb->stack3_ - PGSIZE), PGSIZE));

        // release page directory table
        phy_release_page(pcb->pgdir_pa_);
        bzero(pcb, sizeof(pcb_t));

    } // end for()
}
