/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "tasks.h"

static node_t __idle_node;
node_t __temp_node;
queue_t __queue_ready, __queue_running;
static size_t __global_tid;
static spinlock_t __spinlock_alloc_tid;                     // serially access thread id
static spinlock_t __spinlock_tasks_queue;                   // serially access task queues

/**
 * @brief Get the thread id
 * 
 * @return id
 */
static size_t
get_tid() {
    size_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = ++__global_tid;
    signal(&__spinlock_alloc_tid);

    return temp;
}

/**
 * @brief initialize the tasks queue
 */
void
init_tasks_queue() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    spinlock_init(&__spinlock_alloc_tid);
    spinlock_init(&__spinlock_tasks_queue);
}

/**
 * @brief construct the idle thread
 */
void
create_kernel_idle() {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack

    pcb_t *idle_pcb = (pcb_t *)(STACK_BOOT - STACK_BOOT_SIZE);
    set_pcb(idle_pcb, null, (uint32_t *)STACK_BOOT, TIMETICKS);

    bzero(&__idle_node, sizeof(node_t));
    __idle_node.data_ = idle_pcb;                           // idle node points to its pcb
    __idle_node.next_ = null;

    // setup the tasks queue
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_running, &__idle_node, TAIL);
    signal(&__spinlock_tasks_queue);
}

/**
 * @brief fill in pcb
 * 
 * @param pcb       pcb to be filled
 * @param cur_stack current stack
 * @param stack0    ring0 stack
 * @param ticks     ticks amount
 * @param tid       thread id
 */
void
set_pcb(pcb_t *pcb, uint32_t *cur_stack, uint32_t *stack0, uint32_t ticks) {
    bzero(pcb, sizeof(pcb_t));
    pcb->stack_ = cur_stack;
    pcb->stack0_ = stack0;
    pcb->ticks_ = ticks;
    pcb->tid_ = get_tid();
}

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_pcb() {
    wait(&__spinlock_tasks_queue);
    node_t *cur = queue_front(&__queue_running);
    signal(&__spinlock_tasks_queue);
    ASSERT(!cur);

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    ASSERT(!cur_pcb);

    return cur_pcb;
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
        __tss.ss0_ = DS_SELECTOR_KERN;
        __tss.esp0_ = (uint32_t)((pcb_t *)next->data_)->stack0_;
        
        switch_to(cur, next);
    }
    signal(&__spinlock_tasks_queue);
}

/**
 * @brief make current task to sleep
 * 
 * @param q the sleeping queue
 */
void
sleep(queue_t *q) {
    wait(&__spinlock_tasks_queue);
    ASSERT(!queue_front(&__queue_running));

    // drop current tasks and enqueue sleep queue

    node_t *cur = queue_pop(&__queue_running);
    if (cur)    queue_push(q, cur, TAIL);
    signal(&__spinlock_tasks_queue);
}

/**
 * @brief wakeup a thread of the specified channel
 * 
 * @param q the sleeping queue
 */
void
wakeup(queue_t *q) {
    ASSERT(!queue_front(q));

    node_t *cur = queue_pop(q);
    if (cur) {
        wait(&__spinlock_tasks_queue);
        queue_push(&__queue_ready, cur, TAIL);
        signal(&__spinlock_tasks_queue);
    }
}

/**
 * @brief Create a kernel thread (stack always assumed 4MB)
 * if ring-3 stack specified, then is a user mode thread
 * 
 * @param r0_top top of ring 0 stack
 * @param r3_top top of ring 3 stack
 * @param entry  thread entry
 */
void
create_kthread(uint8_t *r0_top, uint8_t *r3_top, void *entry) {
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
    workercpu->vec_ = 0;
    workercpu->errcode_ = 0;
    workercpu->oldeip_ = r3_top ?
        (uint32_t *)mode_ring3 : (uint32_t *)entry;
    workercpu->oldcs_ = CS_SELECTOR_KERN;
    workercpu->eflags_ = EFLAGS_IF;                         // the new task will enable interrupt
    workeros->edi_ = 0;
    workeros->esi_ = 0;
    workeros->ebp_ = 0;
    workeros->esp_ = (uint32_t)(((uint32_t *)workercpu) - 5);// skip the 5 segment regs
    workeros->ebx_ = 0;
    workeros->edx_ = 0;
    workeros->ecx_ = 0;
    workeros->eax_ = 0;
    workeros->gs_ = DS_SELECTOR_KERN;
    workeros->fs_ = DS_SELECTOR_KERN;
    workeros->es_ = DS_SELECTOR_KERN;
    workeros->ds_ = DS_SELECTOR_KERN;
    workerth->retaddr_ = isr_part3;

    // setup the thread pcb
    pcb_t *pcb = (pcb_t *)r0_top;                           // pcb lies at the bottom of the stack
    set_pcb(pcb, (uint32_t *)pstack,
        (uint32_t *)((uint32_t)r0_top + PGSIZE), TIMETICKS);

    // setup to the ready queue waiting to execute
    bzero(&__temp_node, sizeof(node_t));
    __temp_node.data_ = pcb;
    __temp_node.next_ = null;
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_ready, &__temp_node, TAIL);
    signal(&__spinlock_tasks_queue);
}
