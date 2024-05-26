/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_irq.h"

static node_t __mdata_node[MAX_TASKS_AMOUNT];
// representation of FIFO disk req
static queue_t __queue_ata;
// control the accessing of `__queue_ata`
static spinlock_t __spinlock_ata;
static sleeplock_t __sleeplock_ata;

/**
 * @brief ata initialization with irq method
 */
void
ata_irq_init() {
    bzero(__mdata_node, sizeof(__mdata_node));
    queue_init(&__queue_ata);
    spinlock_init(&__spinlock_ata);
    sleeplock_init(&__sleeplock_ata);
}

/**
 * @brief ata interrupt handler
 */
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
            ata_space_t *ata_space = get_ataspace();
            insw(buff->buff_, buff->len_ / sizeof(uint16_t),
                ata_space->device_info_[ata_space->current_select_].io_port_
                + ATA_IO_RW_OFFSET_DATA);
        }
    }

    // 2. wakeup the asleep thread
    wait(&__sleeplock_ata.guard_);
    wakeup(&__sleeplock_ata);
    signal(&__sleeplock_ata.guard_);
}

/**
 * @brief ata io with irq method
 * 
 * @param buff   ata buffer
 * @param is_irq is the method called by irq
 */
void
ata_irq_rw(atabuff_t *buff, bool is_irq) {
    pcb_t *cur_pcb = get_current_pcb();
    ata_space_t *ata_space = get_ataspace();
    uint16_t io_port =
        ata_space->device_info_[ata_space->current_select_].io_port_;

    // 1. stash the buff
    node_t *node = &__mdata_node[cur_pcb->tid_];
    bzero(node, sizeof(node_t));
    node->data_ = buff;
    wait(&__spinlock_ata);
    queue_push(&__queue_ata, node, TAIL);
    signal(&__spinlock_ata);

    // 2. set register
    ata_set_cmd(ata_space->current_select_, buff->lba_,
        buff->len_ / BYTES_SECTOR, buff->cmd_, is_irq);
    if (buff->cmd_ == ATA_CMD_IO_WRITE)
        outsw(buff->buff_, buff->len_ / sizeof(uint16_t),
            ata_space->device_info_[ata_space->current_select_].io_port_
            + ATA_IO_RW_OFFSET_DATA);

    // 3. sleep (give up CPU)
    if (inb(io_port + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_BSY) {
        wait(&__sleeplock_ata.guard_);
        sleep(&__sleeplock_ata);
        signal(&__sleeplock_ata.guard_);
    }
}
