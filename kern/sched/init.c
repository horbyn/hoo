/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "init.h"

queue_t __queue_ready;                                      // wait to schedule
queue_t __queue_running;                                    // scheduling
static node_t __idle_node;
static pcb_t __idle_pcb;
static uint8_t __init_stack[PGSIZE] = { 0 };                // the stack used by init thread
static node_t __init_node;
static pcb_t __init_pcb;

/**
 * @brief initialize the tasks queue
 */
void
init_tasks_queue() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
}

/**
 * @brief construct the idle thread
 */
void
kernel_idle_thread() {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack

    //uint8_t *pstack = (uint8_t *)STACK_BOOT;
    //pstack -= sizeof(uint32_t) * 2;                         // entry stack
    //pstack -= sizeof(uint32_t);                             // return addr
    //pstack -= sizeof(uint32_t) * 3;                         // calling convention
    //pstack -= sizeof(istackcpu_t);
    //pstack -= sizeof(istackos_t);
    //pstack -= sizeof(tstack_t);

    bzero(&__idle_pcb, sizeof(pcb_t));
    //__idle_pcb.thread_stack_ = (uint32_t *)pstack;

    // setup the tasks queue
    queue_push(&__queue_running, &__idle_node, &__idle_pcb);
}

/**
 * @brief construct the init thread
 */
void
kernel_init_thread() {
    /*
     ************************************
     * kernel stack of idle thread :    *
     * ┌───────────────────────────────┐*
     * │  interrupt stack used by cpu  │*
     * ├───────────────────────────────┤*
     * │  interrupt stack used by os   │*
     * ├───────────────────────────────┤*
     * │         thread stack          │*
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
    pstack -= sizeof(istackcpu_t);
    istackcpu_t *workercpu = (istackcpu_t *)pstack;

    pstack -= sizeof(istackos_t);
    istackos_t *workeros = (istackos_t *)pstack;

    pstack -= sizeof(tstack_t);
    tstack_t *workerth = (tstack_t *)pstack;

    // setup the thread context
    workercpu->vec_ = 0;
    workercpu->errcode_ = 0;
    workercpu->oldeip_ = (uint32_t *)init_thread;
    workercpu->oldcs_ = 0x8;
    workercpu->eflags_ = 0x212;
    workeros->edi_ = 0;
    workeros->esi_ = 0;
    workeros->ebp_ = 0;
    workeros->esp_ = (uint32_t)(((uint32_t *)workercpu) - 5);
    workeros->ebx_ = 0;
    workeros->edx_ = 0;
    workeros->ecx_ = 0;
    workeros->eax_ = 0;
    workeros->ss_ = 0x10;
    workeros->gs_ = 0x10;
    workeros->fs_ = 0x10;
    workeros->es_ = 0x10;
    workeros->ds_ = 0x10;
    workerth->retaddr_ = isr_part3;

    // setup the thread pcb
    bzero(&__init_pcb, sizeof(pcb_t));
    __init_pcb.thread_stack_ = (uint32_t *)pstack;

    // setup to the ready queue waiting to execute
    queue_push(&__queue_ready, &__init_node, &__init_pcb);
}

/**
 * @brief init thread definition
 */
void
init_thread() {
    kprint_char('I');
}
