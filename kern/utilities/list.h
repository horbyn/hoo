/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_LIST_H__
#define __KERN_UTILITIES_LIST_H__

#include "node.h"
#include "kern/types.h"

/**
 * @brief linked list definition
 */
typedef struct list {
    node_t   null_;
    uint32_t size_;
} list_t;

// get the elem index after the tail of the list
#define LSIDX_AFTAIL(list_ptr)  ((list_ptr)->size_ + 1)

void   list_init(list_t *list, bool cycle);
node_t *list_find(list_t *list, int idx);
void   list_insert(list_t *list, node_t *node, int idx);
node_t *list_remove(list_t *list, int idx);
bool   list_isempty(list_t *list);

#endif
