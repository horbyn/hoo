/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_QUEUE_H__
#define __KERN_UTILITIES_QUEUE_H__

#include "node.h"
#include "kern/x86.h"
#include "kern/driver/io.h"

/**
 * @brief definition of queue
 */
typedef struct queue {
    // the head node always in front
    node_t null_;
    node_t *head_, *tail_;
} queue_t;

/**
 * @brief the method to operate the queue
 */
typedef enum enqueue_method {
    TAIL = 0,
    HEAD
} enq_mth_t;

void queue_init(queue_t *q);
bool queue_isempty(queue_t *q);
void queue_push(queue_t *q, node_t *m, enq_mth_t mth);
node_t *queue_pop(queue_t *q);
node_t *queue_front(queue_t *q);

#endif
