/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_QUEUE_H__
#define __KERN_LIB_QUEUE_H__

#include "kern/types.h"
#include "kern/assert.h"

/**
 * @brief definition of queue node
 */
typedef struct node {
    void *data_;                                            // data field
    struct node *next_;
} node_t;

/**
 * @brief definition of queue
 */
typedef struct queue {
    node_t null_;                                           // the head node always in front
    node_t *head_, *tail_;
} queue_t;

typedef enum enqueue_method {
    TAIL = 0,
    HEAD
} enq_mth_t;

void queue_init(queue_t *);
bool queue_isempty(queue_t *);
void queue_push(queue_t *, node_t *, enq_mth_t);
node_t *queue_pop(queue_t *);
node_t *queue_front(queue_t *);

#endif
