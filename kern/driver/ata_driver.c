/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata_driver.h"

static enum_ata_method __ata_driver_method;

/**
 * @brief initialize ata driver
 * 
 * @param method ata method like IRQs or polling
 */
void
ata_driver_init(enum_ata_method method) {
#ifdef DEBUG
    kprintf("ata_driver_init()\n");
#endif

    __ata_driver_method = method;

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_init(); break;
    // polling default
    default: break;
    }
}

/**
 * @brief read sector from ata device
 * 
 * @param buff ata buffer
 */
void
ata_driver_rw(atabuff_t *buff) {
#ifdef DEBUG
    kprintf("ata_driver_rw()\n");
#endif

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_rw(buff); break;
    // polling default
    default: break;
    }

}
