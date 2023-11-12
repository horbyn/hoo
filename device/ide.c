/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ide.h"

queue_t __queue_ide, __queue_ide_sleep;
static spinlock_t __spinlock_ide;                           // control the accessing of `__queue_ide`

/**
 * @brief write command to ide register
 * 
 * @param lba lba no
 * @param cr  the sector amount
 * @param cmd commnd
 */
static void
ide_set_cmd(uint32_t lba, uint8_t cr, ider_cmd_t cmd) {
    // error checking
    while ((inb(IDE_CMDMAP_STATUS_COMMAND) & (BSY | READY)) != READY);
    outb(0, 0x3f6);

    // real operation
    outb(cr, IDE_CMDMAP_SECTORCR);
    outb((uint8_t)(lba & 0xff), IDE_CMDMAP_LBALOW);
    outb((uint8_t)((lba >> 8) & 0xff), IDE_CMDMAP_LBAMID);
    outb((uint8_t)((lba >> 16) & 0xff), IDE_CMDMAP_LBAHIGH);
    outb((uint8_t)(((lba >> 24) & 0xf) | 0xa | MOD_LBA), IDE_CMDMAP_DEVICE);
    outb((uint8_t)cmd, IDE_CMDMAP_STATUS_COMMAND);
}

/**
 * @brief initialize ide
 */
void
ide_init() {
    queue_init(&__queue_ide);
    queue_init(&__queue_ide_sleep);
    spinlock_init(&__spinlock_ide);
}

/**
 * @brief read sector from ide device
 * 
 * @param buff ide buffer
 */
void
ide_rw(idebuff_t *buff) {

    // 1. stash the buff into queue
    node_t node;
    bzero(&node, sizeof(node_t));
    node.data_ = buff;
    wait(&__spinlock_ide);
    queue_push(&__queue_ide, &node, TAIL);
    signal(&__spinlock_ide);

    // 2. set register
    ide_set_cmd(buff->lba_, buff->len_ / BYTES_SECTOR, buff->cmd_);
    if (buff->cmd_ == WRITE)
        outsl(buff->buff_, buff->len_ / 4, IDE_CMDMAP_DATA);

    // 3. sleep (give up CPU)
    sleep(&__queue_ide_sleep);
    scheduler();

}

void
ide_intr() {

    // 1. check the head what command used
    wait(&__spinlock_ide);
    node_t *done = queue_pop(&__queue_ide);
    signal(&__spinlock_ide);
    if (done) {
        idebuff_t *buff = done->data_;
        ASSERT(!buff);

        if (buff->cmd_ == READ)
            insl(buff->buff_, buff->len_ / 4, IDE_CMDMAP_DATA);
    }

    // 2. wakeup the asleep thread
    wakeup(&__queue_ide_sleep);

}

void
idebuff_set(idebuff_t *ibuff, void *buff, size_t len,
size_t lba, ider_cmd_t cmd) {
    ibuff->buff_ = buff;
    ibuff->len_ = len;
    ibuff->lba_ = lba;
    ibuff->cmd_ = cmd;
    ibuff->thread_ = get_pcb()->tid_;
}
