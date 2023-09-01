/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "tasksop.h"

queue_t __queue_ready;                                      // wait to schedule
queue_t __queue_running;                                    // scheduling
static node_t __idle_node, __init_node;
static uint8_t __init_stack[PGSIZE] = { 0 };                // the stack used by init thread
spinlock_t __spinlock_disp;
static uint8_t __init_stack_r3[PGSIZE] = { 0 };
uint32_t user = 0;                                          // used to test user thread switching and will be delete later

/**
 * @brief initialize the tasks queue
 */
void
init_tasks_queue() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
}

/**
 * @brief displaying resource lock initialization
 */
void
init_disp_locks() {
    spinlock_init(&__spinlock_disp);
}

/**
 * @brief construct the idle thread
 */
void
kernel_idle_thread() {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack

    pcb_t *idle_pcb = (pcb_t *)(STACK_BOOT - STACK_BOOT_SIZE);
    bzero(idle_pcb, sizeof(pcb_t));
    idle_pcb->stack0_ = (uint32_t *)STACK_BOOT;

    bzero(&__idle_node, sizeof(node_t));
    __idle_node.data_ = idle_pcb;                           // idle node points to its pcb
    __idle_node.next_ = null;

    // setup the tasks queue
    queue_push(&__queue_running, &__idle_node);
}

/**
 * @brief construct the init thread
 */
void
user_init_thread() {
    /*
     ************************************
     * kernel stack of idle thread :    *
     * ┌───────────────────────────────┐*
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
    uint8_t *pstack = __init_stack + sizeof(__init_stack);

    // always located on the top of new task stack that the `esp`
    // pointed to when the new task completes its initialization
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = DIED_INSTRUCTION;

    // user mode entry
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)init_thread;

    // user mode stack
    pstack -= sizeof(uint32_t);
    *((uint32_t *)pstack) = (uint32_t)__init_stack_r3 + sizeof(__init_stack_r3);

    pstack -= sizeof(istackcpu_t);
    istackcpu_t *workercpu = (istackcpu_t *)pstack;

    pstack -= sizeof(istackos_t);
    istackos_t *workeros = (istackos_t *)pstack;

    pstack -= sizeof(tstack_t);
    tstack_t *workerth = (tstack_t *)pstack;

    // setup the thread context
    workercpu->vec_ = 0;
    workercpu->errcode_ = 0;
    workercpu->oldeip_ = (uint32_t *)mode_ring3;
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
    pcb_t *init_pcb = (pcb_t *)&__init_stack;               // pcb lies at the bottom of the stack
    bzero(init_pcb, sizeof(pcb_t));
    init_pcb->stack_ = (uint32_t *)pstack;
    init_pcb->stack0_ = (uint32_t *)((uint32_t)__init_stack + sizeof(__init_stack));

    // setup to the ready queue waiting to execute
    bzero(&__init_node, sizeof(node_t));
    __init_node.data_ = init_pcb;
    __init_node.next_ = null;
    queue_push(&__queue_ready, &__init_node);
}

/**
 * @brief init thread definition
 */
void
init_thread() {
    while (1) {
        user++;
        printf("%x ", user);
        //printf("User ");
    }
}
