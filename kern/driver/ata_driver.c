/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata_driver.h"

queue_t __queue_ata, __queue_ata_sleep;
static spinlock_t __spinlock_ata;                           // control the accessing of `__queue_ata`

/**
 * @brief write command to ide register
 * 
 * @param lba lba no
 * @param cr  the sector amount
 * @param cmd commnd
 */
static void
ata_set_cmd(uint32_t lba, uint8_t cr, ata_cmd_t cmd) {
#ifdef DEBUG
    kprintf("ata_set_cmd(): %d %d %d\n", lba, cr, cmd);
#endif

    if (!ata_is_selected())
        panic("ata_set_cmd(): not selected device\n");

    port_t io_port =
        ata_space.device_info_[ata_space.current_select_].io_port_;

    // error checking
    ata_wait_not_busy_but_ready();

    ata_disable_irqs();

    // real operation
    outb(cr, io_port + ATA_IO_RW_OFFSET_SECTOR_COUNT);
    outb((uint8_t)(lba & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_LOW);
    outb((uint8_t)((lba >> 8) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_MID);
    outb((uint8_t)((lba >> 16) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_HIGH);
    ata_select(ata_space.current_select_, (uint8_t)((lba >> 24) & 0xf));
    outb((uint8_t)cmd, io_port + ATA_IO_W_OFFSET_COMMAND);

}

/**
 * @brief initialize ata driver
 */
void
ata_driver_init() {
#ifdef DEBUG
    kprintf("ata_driver_init()\n");
#endif
    queue_init(&__queue_ata);
    queue_init(&__queue_ata_sleep);
    spinlock_init(&__spinlock_ata);
}

/**
 * @brief read sector from ata device
 * 
 * @param buff ata buffer
 */
void
ata_rw(atabuff_t *buff) {
#ifdef DEBUG
    kprintf("ata_rw()\n");
#endif

    // 1. stash the buff into queue
    node_t node;
    bzero(&node, sizeof(node_t));
    node.data_ = buff;
    wait(&__spinlock_ata);
    queue_push(&__queue_ata, &node, TAIL);
    signal(&__spinlock_ata);

    // 2. set register
    ata_set_cmd(buff->lba_, buff->len_ / BYTES_SECTOR, buff->cmd_);
    if (buff->cmd_ == ATA_CMD_IO_WRITE)
        outsw(buff->buff_, buff->len_ / 4,
            ata_space.device_info_[ata_space.current_select_].io_port_
            + ATA_IO_RW_OFFSET_DATA);

    // 3. sleep (give up CPU)
    sleep(&__queue_ata_sleep);
    scheduler();

}

void
ata_intr() {
#ifdef DEBUG
    kprintf("ata_intr()\n");
#endif

    // 1. check the head what command used
    wait(&__spinlock_ata);
    node_t *done = queue_pop(&__queue_ata);
    signal(&__spinlock_ata);
    if (done) {
        atabuff_t *buff = done->data_;
        if (!buff)    panic("ata_intr(): idebuff not set");

        if (buff->cmd_ == ATA_CMD_IO_READ) {
            insw(buff->buff_, buff->len_ / 2,
                ata_space.device_info_[ata_space.current_select_].io_port_
                + ATA_IO_RW_OFFSET_DATA);
        }
    }

    // 2. wakeup the asleep thread
        wakeup(&__queue_ata_sleep);

}

void
atabuff_set(atabuff_t *ibuff, void *buff, size_t len,
size_t lba, ata_cmd_t cmd) {
#ifdef DEBUG
    kprintf("atabuff_set(): %x %x %x %x\n", ibuff, buff, len, lba);
#endif
    ibuff->buff_ = buff;
    ibuff->len_ = len;
    ibuff->lba_ = lba;
    ibuff->cmd_ = cmd;
    ibuff->thread_ = get_pcb()->tid_;
}
