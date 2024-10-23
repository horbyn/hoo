/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_polling.h"

/**
 * @brief ata initialization with polling method
 */
void
ata_polling_init(void) {
    // nothing to do
    return;
}

/**
 * @brief ata io with polling method
 * 
 * @param buff   ata buffer
 * @param is_irq is the method called by irq
 */
void
ata_polling_rw(atabuff_t *buff, bool is_irq){
    if (buff == null)    panic("ata_polling_rw(): null pointer");
    if (buff->len_ < BYTES_SECTOR)
        panic("ata_polling_rw(): not meet a sector size");

    if (!is_irq)    ata_disable_irqs();

    uint32_t sectors_to_rw = buff->len_ / BYTES_SECTOR;
    if (buff->len_ % BYTES_SECTOR != 0)    sectors_to_rw++;

    // r/w sectors one by one
    ataspc_t *space = get_ataspace();
    for (uint32_t i = 0; i < sectors_to_rw && buff->len_ > 0; i++) {
        ata_set_cmd(space->current_select_, buff->lba_, 1, buff->cmd_, false);

        uint32_t rest_bytes = (buff->len_ >= BYTES_SECTOR) ?
            (BYTES_SECTOR / sizeof(uint16_t)) : (buff->len_ / sizeof(uint16_t));

        if (buff->cmd_ == ATA_CMD_IO_READ)
            insw(buff->buff_, rest_bytes, ATA_IO_RW_OFFSET_DATA +
                space->device_info_[space->current_select_].io_port_);
        else if (buff->cmd_ == ATA_CMD_IO_WRITE)
            outsw(buff->buff_, rest_bytes, ATA_IO_RW_OFFSET_DATA +
                space->device_info_[space->current_select_].io_port_);
        else    panic("ata_polling_rw(): bug");

        buff->lba_ += 1;
        buff->len_ -= BYTES_SECTOR;
        buff->buff_ += BYTES_SECTOR;
    }

}
