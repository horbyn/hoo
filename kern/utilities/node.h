#pragma once
#ifndef __KERN_UTILITIES_NODE_H__
#define __KERN_UTILITIES_NODE_H__

#include "kern/x86.h"

/**
 * @brief node（用作链表、队列的结点）
 */
typedef struct node {
    void        *data_;
    struct node *next_;
} node_t;

void node_set(node_t *node, void *data, node_t *next);

#endif
