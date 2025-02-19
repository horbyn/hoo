#pragma once
#ifndef __KERN_UTILITIES_QUEUE_H__
#define __KERN_UTILITIES_QUEUE_H__

#include "node.h"

/**
 * @brief 队列
 */
typedef struct queue {
    // 头结点
    node_t null_;
    node_t *head_, *tail_;
} queue_t;

/**
 * @brief 入队方式
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
