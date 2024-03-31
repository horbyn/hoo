/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UNITS_NODE_H__
#define __KERN_UNITS_NODE_H__

#include "kern/x86.h"
#include "kern/driver/io.h"

/**
 * @brief definition of queue node
 */
typedef struct node {
    // data field
    void        *data_;
    struct node *next_;
} __attribute__((packed)) node_t;

void node_set(node_t *node, void *data, node_t *next);

#endif
