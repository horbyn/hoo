/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "blocks.h"

static node_t nodes[NODE_NUM];
static list_t list_blocks;

/**
 * @brief initialize blocks io
 */
void
blocks_init(void) {
    list_init(&list_blocks);
    bzero(nodes, sizeof(nodes));
}
