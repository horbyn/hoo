/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ide.h"

queue_t __queue_ide;

/*static void
ide_set_cmd(uint32_t lba, uint8_t cr, ider_cmd_t cmd) {
    outb(cr, IDE_CMDMAP_SECTORCR);
    outb((uint8_t)(lba & 0xff), IDE_CMDMAP_LBALOW);
    outb((uint8_t)((lba >> 8) & 0xff), IDE_CMDMAP_LBAMID);
    outb((uint8_t)((lba >> 16) & 0xff), IDE_CMDMAP_LBAHIGH);
    outb((uint8_t)(((lba >> 24) & 0xf) | 0xa | MOD_LBA), IDE_CMDMAP_DEVICE);
    outb((uint8_t)cmd, IDE_CMDMAP_STATUS_COMMAND);
}*/

/**
 * @brief initialize ide
 */
void
ide_init() {
    queue_init(&__queue_ide);
}

/**
 * @brief read sector from ide device
 * 
 * @param lba  sector no.
 * @param buff ide buffer
 */
void
ide_rw(uint32_t lba, idebuff_t *buff) {
    // 1. stash the buff into queue
    // 2. set register
    // 3. sleep (give up CPU)
    // 4. wait to wakeup

    node_t node;
    bzero(&node, sizeof(node_t));
    node.data_ = buff;
    queue_push(&__queue_ide, &node, HEAD);
}
