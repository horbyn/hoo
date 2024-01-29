/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "blocks.h"

static node_t nodes[NODE_NUM], node_null;

/**
 * @brief temparory allocate node
 * 
 * @return node
 */
static node_t *
blocks_node_alloc(void) {
    for (size_t i = 0; i < NODE_NUM; ++i) {
        if (memcmp(&nodes[i], &node_null, sizeof(node_t)))
            return &nodes[i];
    }
    return null;
}

/**
 * @brief temparory release node
 */
static void
blocks_node_free(node_t *node) {
    bzero(node, sizeof(node_t));
}

/**
 * @brief initialize blocks io
 */
void
blocks_list_init(blocks_list_t *list) {
    list_init(list);
}

/**
 * @brief Set up block structure
 * 
 * @param block block structure
 * @param block_addr block addr pointed to 512 bytes
 * @param lba lba
 */
void
set_blocks(blocks_t *block, void *block_addr, lba_index_t lba) {
    if (block == null)    panic("set_blocks(): parameter invalid");

    block->block_addr_ = block_addr;
    block->lba_ = lba;
}

/**
 * @brief blocks list adds nodes (MUST BE filled with 512 bytes)
 * 
 * @param list blocks list
 * @param block data to be added
 */
void
blocks_append(blocks_list_t *list, const blocks_t *block) {
    node_t *node = blocks_node_alloc();
    if (node == null)    panic("blocks_append(): node alloc failed");

    node->data_ = block->block_addr_;
    node->next_ = null;
    list_insert(list, node, list->size_ + 1);

}

/**
 * @brief blocks data read from/write to disk (always in 512 bytes)
 * 
 * @param list the blocks list
 * @param cmd read or write operation
 */
void
blocks_rw_disk(blocks_list_t *list, ata_cmd_t cmd) {
    if (list == null)    panic("blocks_rw_disk(): parameter invalid");

    node_t *node = list->null_.next_;
    while (node) {
        blocks_t *block = (blocks_t *)node->data_;
        ata_driver_rw(block->block_addr_, BYTES_SECTOR, block->lba_, cmd);
        node_t *next = node->next_;
        blocks_node_free(node);
        node = next;
    }
}
