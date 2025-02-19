#pragma once
#ifndef __KERN_MEM_FORMAT_LIST_H__
#define __KERN_MEM_FORMAT_LIST_H__

#include "kern/utilities/list.h"

/**
 * @brief 格式化链表
 */
typedef struct format_list {
    // 特定类型的大小
    uint32_t           type_size_;
    // 链表
    list_t             list_;
    // 第一个元素的偏移
    uint32_t           first_off_;
    // 容量
    uint32_t           capacity_;
    struct format_list *next_;
} fmtlist_t;

void *fmtlist_alloc(fmtlist_t **fmtlist, uint32_t size);
bool fmtlist_release(fmtlist_t **fmtlist, void *elem, uint32_t elem_size);

#endif
