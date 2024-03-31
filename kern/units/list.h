/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UNITS_LIST_H__
#define __KERN_UNITS_LIST_H__

#include "node.h"

/**
 * @brief linked list definition
 */
typedef struct list {
    node_t   null_;
    uint32_t size_;
} __attribute__((packed)) list_t;

// get the elem index after the tail of the list
#define LSIDX_AFTAIL(list_ptr)  ((list_ptr)->size_ + 1)

void   list_init(list_t *list, bool cycle);
node_t *list_find(list_t *list, idx_t idx);
void   list_insert(list_t *list, node_t *node, idx_t idx);
node_t *list_remove(list_t *list, idx_t idx);
bool   list_isempty(list_t *list);

#endif
