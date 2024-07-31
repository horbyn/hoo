/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "circular_buffer.h"

/**
 * @brief allocate circular buffer
 * 
 * @param capacity the capacity of the buffer
 * @return cclbuff object
 */
cclbuff_t *
cclbuff_alloc(uint32_t capacity) {
    cclbuff_t *cclbuff = dyn_alloc(sizeof(cclbuff_t));
    cclbuff->capacity_ = capacity;
    cclbuff->buff_ = dyn_alloc(capacity);
    cclbuff->head_ = cclbuff->tail_ = 0;
    sleeplock_init(&cclbuff->slock_);
    return cclbuff;
}

/**
 * @brief release circular buffer
 * 
 * @param cclbuff the cclbuff object
 */
void
cclbuff_free(cclbuff_t *cclbuff) {
    if (cclbuff == null)    return;
    if (cclbuff->buff_)    dyn_free(cclbuff->buff_);
    dyn_free(cclbuff);
}

/**
 * @brief whether the buffer is empty
 * 
 * @param cclbuff circular buffer
 * @retval true: empty
 * @retval false: non-empty
 */
static bool
cclbuff_isempty(cclbuff_t *cclbuff) {
    return (cclbuff->head_ == cclbuff->tail_);
}

/**
 * @brief whether the buffer is full
 * 
 * @param cclbuff circular buffer
 * @retval true: full
 * @retval false: non-full
 */
static bool
cclbuff_isfull(cclbuff_t *cclbuff) {
    return ((cclbuff->head_ + 1) % cclbuff->capacity_ == cclbuff->tail_);
}

/**
 * @brief a character put into the circular buffer
 * 
 * @param cclbuff circular buffer
 * @param c       a character
 * @retval true: succeed
 * @retval false: fail
 */
bool
cclbuff_put(cclbuff_t *cclbuff, char c) {
    if (cclbuff == null)    panic("cclbuff_put(): null pointer");

    // as a productor
    if (cclbuff_isfull(cclbuff))    return false;
    cclbuff->buff_[cclbuff->head_] = c;
    cclbuff->head_ = (cclbuff->head_ + 1) % cclbuff->capacity_;
    wait(&cclbuff->slock_.guard_);
    wakeup(&cclbuff->slock_);
    signal(&cclbuff->slock_.guard_);
    return true;
}

/**
 * @brief a character get from the circular buffer
 * 
 * @param cclbuff circular buffer
 * @return character
 */
char
cclbuff_get(cclbuff_t *cclbuff) {
    if (cclbuff == null)    panic("cclbuff_get(): null pointer");

    // as a consumer
    if (cclbuff_isempty(cclbuff)) {
        wait(&cclbuff->slock_.guard_);
        sleep(&cclbuff->slock_);
        signal(&cclbuff->slock_.guard_);
    }
    char ch = cclbuff->buff_[cclbuff->tail_];
    cclbuff->tail_ = (cclbuff->tail_ + 1) % cclbuff->capacity_;
    return ch;
}
