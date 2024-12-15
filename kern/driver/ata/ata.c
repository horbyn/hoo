/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata.h"
#include "ata_irq.h"
#include "ata_polling.h"
#include "kern/panic.h"
#include "kern/module/log.h"
#include "kern/sched/pcb.h"
#include "kern/sched/tasks.h"

static atabuff_t __mdata_atabuff[MAX_TASKS_AMOUNT];
static atamth_t __ata_driver_method;

/**
 * @brief ata device initialization
 */
void
init_ata(void) {
    ata_detect();

    ataspc_t *ata_space = get_ataspace();
    for (uint32_t i = 0; i < ata_space->device_amount_; ++i) {
        klog_write("================ DEVICE  INFO ================"
            "\nbus:           %s"
            "\nbus wire:      %s"
            "\ntype:          %s"
            "\nio port:       0x%x"
            "\nctrl port:     0x%x"
            "\ndev no.:       %d"
            "\ntotal sectors: %d"
            "\nserial no.:    %s"
            "\nmodel no.:     %s\n\n",
            MACRO2STR_ATA_TYPE_BUS(ata_space->device_info_[i].bus_),
            MACRO2STR_ATA_TYPE_BUS_WIRE(ata_space->device_info_[i].bus_wire_),
            MACRO2STR_ATA_TYPE_DEVICE(
                ata_space->device_info_[i].device_type_),
            ata_space->device_info_[i].io_port_,
            ata_space->device_info_[i].ctrl_port_,
            ata_space->device_info_[i].device_no_,
            ata_space->device_info_[i].total_sectors_,
            (char *)&(ata_space->device_info_[i].dev_serial_),
            (char *)&(ata_space->device_info_[i].dev_model_));
    }

    for (uint32_t i = 0; i < ata_space->device_amount_; ++i) {
        if (ata_space->device_info_[i].valid_) {
            ata_space->current_select_ = i;
            break;
        }
    }
}

/**
 * @brief initialize ata driver
 * 
 * @param method ata method like IRQs or polling
 */
void
ata_driver_change_mode(atamth_t method) {

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
ata_driver_rw(void *buff, uint32_t bufflen, int lba, atacmd_t cmd) {
    if (lba == INVALID_INDEX)
        panic("ata_driver_rw(): invalid lba");
    if (buff == null)    panic("ata_driver_rw(): null pointer");

    pcb_t *cur_pcb = get_current_pcb();
    atabuff_t *atabuff = __mdata_atabuff + cur_pcb->tid_;
    atabuff_set(atabuff, buff, bufflen, lba, cmd);

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_rw(atabuff); break;
    // polling default
    default: ata_polling_rw(atabuff); break;
    }

}
