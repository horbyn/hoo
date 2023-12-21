/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata_irq.h"

queue_t __queue_ata;                                        // representation of FIFO disk req
queue_t __queue_ata_sleep;                                  // disk requests slept here
static spinlock_t __spinlock_ata;                           // control the accessing of `__queue_ata`

void
ata_irq_init() {

    queue_init(&__queue_ata);
    queue_init(&__queue_ata_sleep);
    spinlock_init(&__spinlock_ata);
}

void
ata_irq_intr() {

    // 1. check the head what command used
    wait(&__spinlock_ata);
    node_t *done = queue_pop(&__queue_ata);
    signal(&__spinlock_ata);
    if (done) {
        atabuff_t *buff = done->data_;
        if (!buff)    panic("ata_irq_intr(): idebuff not set");

        if (buff->cmd_ == ATA_CMD_IO_READ) {
            insw(buff->buff_, buff->len_ / sizeof(uint16_t),
                ata_space.device_info_[ata_space.current_select_].io_port_
                + ATA_IO_RW_OFFSET_DATA);
        }
    }

    // 2. wakeup the asleep thread
    if (!queue_isempty(&__queue_ata_sleep))
        wakeup(&__queue_ata_sleep);

}

void
ata_irq_rw(atabuff_t *buff, bool is_irq) {

    // 1. stash the buff into queue
    node_t node;
    bzero(&node, sizeof(node_t));
    node.data_ = buff;
    wait(&__spinlock_ata);
    queue_push(&__queue_ata, &node, TAIL);
    signal(&__spinlock_ata);

    // 2. set register
    ata_set_cmd(ata_space.current_select_, buff->lba_,
        buff->len_ / BYTES_SECTOR, buff->cmd_, is_irq);
    if (buff->cmd_ == ATA_CMD_IO_WRITE)
        outsw(buff->buff_, buff->len_ / sizeof(uint16_t),
            ata_space.device_info_[ata_space.current_select_].io_port_
            + ATA_IO_RW_OFFSET_DATA);

    // 3. sleep (give up CPU)
    if (!queue_isempty(&__queue_ata))
        sleep(&__queue_ata_sleep);
}
