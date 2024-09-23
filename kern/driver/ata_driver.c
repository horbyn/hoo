/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_driver.h"

static atabuff_t __mdata_atabuff[MAX_TASKS_AMOUNT];
static enum_ata_method __ata_driver_method;

/**
 * @brief initialize ata driver
 * 
 * @param method ata method like IRQs or polling
 */
void
ata_driver_change_mode(enum_ata_method method) {

    __ata_driver_method = method;

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_init(); break;
    // polling default
    default: ata_polling_init(); break;
    }
}

/**
 * @brief read sector from ata device
 * 
 * @param buff buff to read/write
 * @param bufflen buff length
 * @param lba lba buff read from/write to
 * @param cmd operation commands
 */
void
ata_driver_rw(void *buff, uint32_t bufflen, idx_t lba, ata_cmd_t cmd) {
    if (lba == INVALID_INDEX)
        panic("ata_driver_rw(): invalid lba");
    if (buff == null)    panic("ata_driver_rw(): null pointer");

    pcb_t *cur_pcb = get_current_pcb();
    atabuff_t *atabuff = &__mdata_atabuff[cur_pcb->tid_];
    atabuff_set(atabuff, buff, bufflen, lba, cmd);

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_rw(atabuff, true); break;
    // polling default
    default: ata_polling_rw(atabuff, false); break;
    }

}
