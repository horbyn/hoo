/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

#include "inte_stack.h"
#include "kern/debug.h"
#include "kern/lib/queue.h"
#include "kern/lib/lib.h"
#include "kern/lib/spinlock.h"
#include "kern/mem/vaddr.h"
#include "kern/module/config.h"

extern void switch_to(node_t *, node_t *);
extern void mode_ring3(uint32_t *, void *);
extern void isr_part3();
extern queue_t __queue_ready;                               // wait to schedule
extern queue_t __queue_running;                             // scheduling
extern node_t __temp_node;                                  // temp node for testing user thread

/**
 * @brief definition of Process Control Block
 */
typedef struct pcb {
    uint32_t *stack_;                                       // either kernel stack or user stack
    uint32_t *stack0_;                                      // kernel stack
    uint32_t ticks_;                                        // the rest ticks
    uint32_t tid_;                                          // thread id
    vaddr_list_t vspace_;                                   // virtual space manager
} pcb_t;

#define TIMETICKS   16                                      // switch when the amount arrived

void init_tasks_queue();
void create_kernel_idle();
void set_pcb(pcb_t *, uint32_t *, uint32_t *, uint32_t);
pcb_t *get_pcb();
void scheduler();
void sleep(queue_t *);
void wakeup(queue_t *);
void create_kthread(uint8_t *, uint8_t *, void *);

#endif
