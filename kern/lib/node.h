/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_NODE_H__
#define __KERN_LIB_NODE_H__

/**
 * @brief definition of queue node
 */
typedef struct node {
    void *data_;                                            // data field
    struct node *next_;
} __attribute__((packed)) node_t;

#endif
