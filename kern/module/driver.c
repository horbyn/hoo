/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "driver.h"

static atabuff_t __mdata_atabuff[MAX_TASKS_AMOUNT];
static enum_ata_method __ata_driver_method;

/**
 * @brief pic initialization
 */
static void
init_pic(void) {
    set_icw1(ICW1_ICW4);                                    // level; cascade; icw4
    set_icw2(0x20, 0x28);
    set_icw3(2);
    set_icw4(ICW4_AUTO);                                    // CFNM; no-buffer; normal EOI

    enable_mask_ocw1(IRQ_TIMER);                            // irq#0
    disable_mask_ocw1(IRQ_KEYBOARD);                        // irq#1
    enable_mask_ocw1(IRQ_CASCADE);                          // irq#2
    disable_mask_ocw1(IRQ_COM2);                            // irq#3
    disable_mask_ocw1(IRQ_COM1);                            // irq#4
    disable_mask_ocw1(IRQ_LPT2);                            // irq#5
    disable_mask_ocw1(IRQ_FLOPPY);                          // irq#6
    disable_mask_ocw1(IRQ_LPT1);                            // irq#7
    disable_mask_ocw1(IRQ_CMOS_RTC);                        // irq#8
    disable_mask_ocw1(IRQ_ACPI);                            // irq#9
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC1);                  // irq#10
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC2);                  // irq#11
    disable_mask_ocw1(IRQ_MOUSE);                           // irq#12
    disable_mask_ocw1(IRQ_MATH);                            // irq#13
    enable_mask_ocw1(IRQ_ATA1);                             // irq#14
    disable_mask_ocw1(IRQ_ATA2);                            // irq#15
}

/**
 * @brief pit initialization
 */
static void
init_pit(void) {
    set_command(SC_CHANNEL0, LOWHIGHBYTE, M3, BINARY);
    set_counter(TICKS_PER_SEC);                             // n singals per second
}

/**
 * @brief ata initialization
*/
static void
init_ata(void) {
    ata_space_init();

    // select the first valid device (device no. is 0 default)
    ata_space_t *ata_space = get_ataspace();
    for (uint32_t i = 0; i < ata_space->device_amount_; ++i) {
        if (ata_space->device_info_[i].valid_) {
            ata_space->current_select_ = i;
            break;
        }
    }
}

/**
 * @brief driver initialization
 */
void
kinit_dirver(void) {
    init_pic();
    init_pit();
    init_ata();
    bzero(__mdata_atabuff, sizeof(__mdata_atabuff));
    ata_driver_init(ATA_METHOD_IRQ);
}

/**
 * @brief initialize ata driver
 * 
 * @param method ata method like IRQs or polling
 */
void
ata_driver_init(enum_ata_method method) {

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

    pcb_t *cur_pcb = get_current_pcb();
    atabuff_t *atabuff = &__mdata_atabuff[cur_pcb->tid_];
    atabuff_set(atabuff, buff, bufflen, lba, cmd);

    switch (__ata_driver_method) {
    case ATA_METHOD_IRQ: ata_irq_rw(atabuff, true); break;
    // polling default
    default: ata_polling_rw(atabuff, false); break;
    }

}
