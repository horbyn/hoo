#pragma once
#ifndef __KERN_UNITS_CIRCULAR_BUFFER_H__
#define __KERN_UNITS_CIRCULAR_BUFFER_H__

#include "spinlock.h"

/**
 * @brief 环形缓冲区（必须在调度模块初始化完成后使用）
 */
typedef struct circular_buffer {
    uint32_t    capacity_;
    char        *buff_;
    int         head_;
    int         tail_;
    spinlock_t slock_;
} cclbuff_t;

cclbuff_t *cclbuff_alloc(uint32_t capacity);
void       cclbuff_free(cclbuff_t *cclbuff);
bool       cclbuff_put(cclbuff_t *cclbuff, char c);
char       cclbuff_get(cclbuff_t *cclbuff);

#endif
