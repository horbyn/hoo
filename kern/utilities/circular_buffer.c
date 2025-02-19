#include "circular_buffer.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"

/**
 * @brief 分配环形缓冲区
 * 
 * @param capacity 缓冲区容量
 * @return cclbuff 对象
 */
cclbuff_t *
cclbuff_alloc(uint32_t capacity) {
    cclbuff_t *cclbuff = dyn_alloc(sizeof(cclbuff_t));
    cclbuff->capacity_ = capacity;
    cclbuff->buff_ = dyn_alloc(capacity);
    cclbuff->head_ = cclbuff->tail_ = 0;
    spinlock_init(&cclbuff->slock_);
    return cclbuff;
}

/**
 * @brief 释放环形缓冲区
 * 
 * @param cclbuff cclbuff 对象
 */
void
cclbuff_free(cclbuff_t *cclbuff) {
    if (cclbuff == null)    return;
    if (cclbuff->buff_)    dyn_free(cclbuff->buff_);
    dyn_free(cclbuff);
}

/**
 * @brief 判断环形缓冲区是否空
 * 
 * @param cclbuff 环形缓冲区
 * @retval true:  空
 * @retval false: 非空
 */
static bool
cclbuff_isempty(cclbuff_t *cclbuff) {
    return (cclbuff->head_ == cclbuff->tail_);
}

/**
 * @brief 环形缓冲区是否满
 * 
 * @param cclbuff 环形缓冲区
 * @retval true:  满
 * @retval false: 未满
 */
static bool
cclbuff_isfull(cclbuff_t *cclbuff) {
    return ((cclbuff->head_ + 1) % cclbuff->capacity_ == cclbuff->tail_);
}

/**
 * @brief 将一个字符写入环形缓冲区
 * 
 * @param cclbuff 环形缓冲区
 * @param c       一个字符
 * @retval true:  成功
 * @retval false: 失败
 */
bool
cclbuff_put(cclbuff_t *cclbuff, char c) {
    if (cclbuff == null)    panic("cclbuff_put(): null pointer");

    // 作为生产者
    if (cclbuff_isfull(cclbuff))    return false;
    cclbuff->buff_[cclbuff->head_] = c;
    cclbuff->head_ = (cclbuff->head_ + 1) % cclbuff->capacity_;
    wakeup(cclbuff);
    return true;
}

/**
 * @brief 从环形缓冲区中读取一个字符
 * 
 * @param cclbuff 环形缓冲区
 * @return 字符
 */
char
cclbuff_get(cclbuff_t *cclbuff) {
    if (cclbuff == null)    panic("cclbuff_get(): null pointer");

    // 作为消费者
    if (cclbuff_isempty(cclbuff)) {
        wait(&cclbuff->slock_);
        sleep(cclbuff, &cclbuff->slock_);
        signal(&cclbuff->slock_);
    }
    char ch = cclbuff->buff_[cclbuff->tail_];
    cclbuff->tail_ = (cclbuff->tail_ + 1) % cclbuff->capacity_;
    return ch;
}
