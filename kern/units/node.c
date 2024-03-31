/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "node.h"

/**
 * @brief setup the node object
 * 
 * @param node the node object
 * @param data the data wrapped by the node
 * @param next next node
 */
void
node_set(node_t *node, void *data, node_t *next) {
    if (node == null)    panic("node_set(): invalid parameter");
    node->data_ = data;
    node->next_ = next;
}
