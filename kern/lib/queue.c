/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "queue.h"

/**
 * @brief initialize the queue
 * @param q the queue needed to operate
 */
void
queue_init(queue_t *q) {
    ASSERT(q == null);

    q->null_.data_ = null;
    q->null_.next_ = null;

    q->head_ = q->tail_ = &q->null_;
}

/**
 * @brief check if queue is empty
 * 
 * @param q the queue needed to operate
 * @retval true the queue is empty
 * @retval false the queue is not empty
 */
bool
queue_isempty(queue_t *q) {
    ASSERT(q == null);

    return (q->head_ == q->tail_);
}

/**
 * @brief enqueuing (tail-enqueue)
 * 
 * @param q the queue needed to operate
 * @param n the enqueuing node
 */
void
queue_push(queue_t *q, node_t *n) {
    ASSERT(q == null || n == null);

    n->next_ = null;

    q->tail_->next_ = n;
    q->tail_ = n;
}

/**
 * @brief dequeuing
 * 
 * @param q the queue needed to operate
 * @retval null the queue is empty
 */
node_t *
queue_pop(queue_t *q) {
    ASSERT(q == null);

    if (queue_isempty(q))    return null;

    node_t *del = q->head_->next_;
    q->head_->next_ = del->next_;
    del->next_ = null;

    if (del == q->tail_)    q->tail_ = q->head_;

    return del;
}
