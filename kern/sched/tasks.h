/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

#include "kern/assert.h"
#include "kern/types.h"
#include "kern/lib/queue.h"
#include "kern/lib/lib.h"
#include "kern/lib/spinlock.h"

extern void switch_to(node_t *, node_t *);
extern void mode_ring3(uint32_t *, void *);
extern void isr_part3();
extern queue_t __queue_ready;                               // wait to schedule
extern queue_t __queue_running;                             // scheduling
extern node_t __temp_node;                                  // temp node for testing user thread

/**
 * @brief definition of os interrupt stack corresponding
 * the order of storing context in `kern/inter/trampoline.s`
 */
typedef struct interrupt_stack_os {
    uint32_t edi_;
    uint32_t esi_;
    uint32_t ebp_;
    uint32_t esp_;
    uint32_t ebx_;
    uint32_t edx_;
    uint32_t ecx_;
    uint32_t eax_;
    uint32_t gs_;
    uint32_t fs_;
    uint32_t es_;
    uint32_t ds_;
} __attribute__ ((packed)) istackos_t;

/**
 * @brief definition of cpu interrupt stack that builds
 * the env the cpu executes `iret` instruction
 */
typedef struct interrupt_stack_cpu {
    uint32_t vec_;                                          // unused when `iret`ing
    uint32_t errcode_;                                      // unused when `iret`ing
    void    *oldeip_;
    uint32_t oldcs_;
    uint32_t eflags_;

} __attribute__ ((packed)) istackcpu_t;

/**
 * @brief thread stack
 */
typedef struct thread_stack {
    void *retaddr_;
} tstack_t;

/**
 * @brief definition of Process Control Block
 */
typedef struct pcb {
    uint32_t *stack_;                                       // either kernel stack or user stack
    uint32_t *stack0_;                                      // kernel stack
    uint32_t ticks_;                                        // the rest ticks
    uint32_t tid_;                                          // thread id
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
