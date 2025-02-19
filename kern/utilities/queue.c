#include "queue.h"
#include "kern/panic.h"

/**
 * @brief 初始化队列
 * @param q 队列
 */
void
queue_init(queue_t *q) {
    if (q == null)    panic("queue_init(): null pointer");

    q->null_.data_ = null;
    q->null_.next_ = null;

    q->head_ = q->tail_ = &q->null_;
}

/**
 * @brief 判断队列空
 * 
 * @param q 队列
 * @retval true  队列空
 * @retval false 队列非空
 */
bool
queue_isempty(queue_t *q) {
    if (q == null)    panic("queue_isempty(): null pointer");

    return (q->head_ == q->tail_);
}

/**
 * @brief 入队
 * 
 * @param q 队列
 * @param n 入队结点
 * @param mth 入队方式（头插入和尾插入）
 */
void
queue_push(queue_t *q, node_t *n, enq_mth_t mth) {
    if (q == null || n == null)
        panic("queue_push(): null pointer");

    n->next_ = null;

    if (mth == HEAD && !queue_isempty(q)) {
        // 头插入且队列空时，和尾插入一样
        n->next_ = q->head_->next_->next_;
        q->head_->next_->next_ = n;
    } else {
        q->tail_->next_ = n;
        q->tail_ = n;
    }
}

/**
 * @brief 出队
 * 
 * @param q 队列
 * @retval 返回 null 如果队列为空
 */
node_t *
queue_pop(queue_t *q) {
    if (q == null)    panic("queue_pop(): null pointer");

    if (queue_isempty(q))    return null;

    node_t *del = q->head_->next_;
    q->head_->next_ = del->next_;
    del->next_ = null;

    if (del == q->tail_)    q->tail_ = q->head_;

    return del;
}

/**
 * @brief 检查队头
 * 
 * @param q 队列
 * @return 第一个结点；或者返回 null 如果队列空
 */
node_t *
queue_front(queue_t *q) {
    if (q == null)    panic("queue_front(): null pointer");

    if (queue_isempty(q))    return null;
    return q->head_->next_;
}
