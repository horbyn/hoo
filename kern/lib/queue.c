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
    return (q->head_ == q->tail_);
}

/**
 * @brief enqueuing
 * 
 * @param q the queue needed to operate
 * @param n the enqueuing node
 * @param d the data the node `n` will be pointed to
 */
void
queue_push(queue_t *q, node_t *n, void *d) {
    n->data_ = d;
    n->next_ = null;

    q->tail_->next_ = n;
    q->tail_ = d;
}

/**
 * @brief dequeuing
 * 
 * @param q the queue needed to operate
 * @retval null the queue is empty
 */
node_t *
queue_pop(queue_t *q) {
    if (queue_isempty(q))    return null;

    node_t *ret = q->head_;
    q->head_ = q->head_->next_;
    return ret;
}
