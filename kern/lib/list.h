/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_LIST_H__
#define __KERN_LIB_LIST_H__

#include "node.h"
#include "kern/types.h"
#include "kern/debug.h"

/**
 * @brief linked list definition
 */
typedef struct list {
    node_t  null_;
    size_t  size_;
} __attribute__((packed)) list_t;

void list_init(list_t *list);
node_t *list_find(list_t *list, idx_t idx);
void list_insert(list_t *list, node_t *node, idx_t idx);
node_t *list_remove(list_t *list, idx_t idx);

#endif
