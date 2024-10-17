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
    if (node == null)    panic("node_set(): null pointer");
    if (data == null)    node->data_ = null;
    else    node->data_ = data;
    if (next == null)    node->next_ = null;
    else    node->next_ = next;
}
