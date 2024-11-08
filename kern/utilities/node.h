/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_NODE_H__
#define __KERN_UTILITIES_NODE_H__

#include "kern/x86.h"

/**
 * @brief definition of node
 */
typedef struct node {
    void        *data_;
    struct node *next_;
} node_t;

void node_set(node_t *node, void *data, node_t *next);

#endif
