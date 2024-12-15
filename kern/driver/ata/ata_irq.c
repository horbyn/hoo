/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_irq.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"
#include "kern/sched/pcb.h"
#include "kern/sched/tasks.h"
#include "kern/utilities/queue.h"
#include "kern/utilities/sleeplock.h"
#include "user/lib.h"

static node_t *__mdata_node;
// representation of FIFO disk req
static queue_t __queue_ata;
static spinlock_t __slqueue, __slata;

/**
 * @brief ata initialization with irq method
 */
void
ata_irq_init() {
    __mdata_node = dyn_alloc(MAX_TASKS_AMOUNT * sizeof(node_t));
    bzero(__mdata_node, sizeof(__mdata_node));
    queue_init(&__queue_ata);
    spinlock_init(&__slqueue);
    spinlock_init(&__slata);
}

/**
 * @brief ata interrupt handler
 */
void
ata_irq_intr() {

    // 1. check the head what command used
    wait(&__slqueue);
    node_t *done = queue_pop(&__queue_ata);
    signal(&__slqueue);
    if (done != null) {
        atabuff_t *buff = done->data_;
        if (!buff)    panic("ata_irq_intr(): idebuff not set");

        if (buff->cmd_ == ATA_CMD_IO_READ) {
            ataspc_t *ata_space = get_ataspace();
            insw(buff->buff_, buff->len_ / sizeof(uint16_t),
                ata_space->device_info_[ata_space->current_select_].io_port_
                + ATA_IO_RW_OFFSET_DATA);
        }
        buff->finish_ = true;
    }

    // 2. wakeup the asleep thread
    wakeup(done);
}

/**
 * @brief ata io with irq method
 * 
 * @param buff   ata buffer
 */
void
ata_irq_rw(atabuff_t *buff) {
    if (buff == null)    panic("ata_irq_rw(): null pointer");
    pcb_t *cur_pcb = get_current_pcb();
    ataspc_t *ata_space = get_ataspace();

    // 1. stash the buff
    node_t *node = __mdata_node + cur_pcb->tid_;
    bzero(node, sizeof(node_t));
    node->data_ = buff;
    wait(&__slqueue);
    queue_push(&__queue_ata, node, TAIL);
    signal(&__slqueue);

    // 2. set register
    ata_set_cmd(ata_space->current_select_, buff->lba_,
        buff->len_ / BYTES_SECTOR, buff->cmd_);
    if (buff->cmd_ == ATA_CMD_IO_WRITE)
        outsw(buff->buff_, buff->len_ / sizeof(uint16_t),
            ata_space->device_info_[ata_space->current_select_].io_port_
            + ATA_IO_RW_OFFSET_DATA);

    // 3. sleep (give up CPU)
    wait(&__slata);
    while (buff->finish_ == false)
        sleep(node, &__slata);
    signal(&__slata);
}
