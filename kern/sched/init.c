/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "init.h"

static queue_t __queue_tasks;
static uint8_t __idle_stack[PGSIZE] = { 0 };                // the stack used by idle thread
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
    queue_init(&__queue_tasks);
}

/**
 * @brief construct the idle thread
 */
void
kernel_idle_thread() {
    /*
     ************************************
     * kernel stack of idle thread :    *
     * ┌───────────────────────────────┐*
     * │  interrupt stack used by cpu  │*
     * ├───────────────────────────────┤*
     * │  interrupt stack used by os   │*
     * ├───────────────────────────────┤*
     * │                               │*
     * │          thread stack         │*
     * │                               │*
     * ├───────────────────────────────┤*
     * │              pcb              │*
     * └───────────────────────────────┘*
     ************************************/

    uint8_t *pstack = __idle_stack + sizeof(__idle_stack);
    pstack -= sizeof(istackcpu_t);
    pstack -= sizeof(istackos_t);

    bzero(&__idle_pcb, sizeof(pcb_t));
    __idle_pcb.thread_stack_ = (uint32_t *)pstack;

    // setup the tasks queue
    queue_push(&__queue_tasks, &__idle_node, &__idle_pcb);
}

/**
 * @brief construct the init thread
 */
void
kernel_init_thread() {
    uint8_t *pstack = __init_stack + sizeof(__init_stack);
    pstack -= sizeof(istackcpu_t);
    pstack -= sizeof(istackos_t);

    bzero(&__init_pcb, sizeof(pcb_t));
    __init_pcb.thread_stack_ = (uint32_t *)pstack;

    queue_push(&__queue_tasks, &__init_node, &__init_pcb);
}
