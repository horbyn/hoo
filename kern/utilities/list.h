#pragma once
#ifndef __KERN_UTILITIES_LIST_H__
#define __KERN_UTILITIES_LIST_H__

#include "node.h"
#include "user/types.h"

/**
 * @brief 单链表
 */
typedef struct list {
    node_t   null_;
    uint32_t size_;
} list_t;

// 链表尾后元素（相当于 C++ STL end()）下标
#define LSIDX_AFTAIL(list_ptr)  ((list_ptr)->size_ + 1)

void   list_init(list_t *list, bool cycle);
node_t *list_find(list_t *list, int idx);
void   list_insert(list_t *list, node_t *node, int idx);
node_t *list_remove(list_t *list, int idx);
bool   list_isempty(list_t *list);

#endif
