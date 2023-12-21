/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata.h"

ata_space_t ata_space;
static ata_device_t ata_devices[ATA_MAX_SUPPORTED_DEVICES];

/**
 * @brief wait 400 ns
 * 
 * @param reg alternate register
 */
static void
ata_wait_register_400ns(uint16_t reg) {
    inb(reg);
    inb(reg);
    inb(reg);
    inb(reg);
}

/**
 * @brief check if device invalid
 * 
 * @param dev_no device no.
 */
static void
ata_check_invalid(uint32_t dev_no) {
    if (ata_space.device_amount_ != 0 && dev_no >= ata_space.device_amount_)
        panic("ata_check_invalid(): device invalid");
}

/**
 * @brief setup ata buffer
 * 
 * @param ibuff ata buffer pointer
 * @param buff  ata data read from/write to device
 * @param len   ata data length
 * @param lba   lba begins from 0
 * @param cmd   read/write
 */
void
atabuff_set(atabuff_t *ibuff, void *buff, size_t len,
size_t lba, ata_cmd_t cmd) {

    ibuff->buff_ = buff;
    ibuff->len_ = len;
    ibuff->lba_ = lba;
    ibuff->cmd_ = cmd;
    ibuff->thread_ = get_pcb()->tid_;
}

/**
 * @brief write command to ide register
 * 
 * @param dev device no.
 * @param lba lba no.
 * @param cr  the sector amount
 * @param cmd commnd
 */
void
ata_set_cmd(uint32_t dev, uint32_t lba, uint8_t cr, ata_cmd_t cmd, bool is_irq) {

    ata_check_invalid(dev);

    port_t io_port =
        ata_space.device_info_[dev].io_port_;

    if (is_irq)    ata_enable_irqs();
    else    ata_disable_irqs();

    // select device
    uint8_t master =
        (ata_devices[dev].bus_wire_ == ATA_TYPE_BUS_WIRE_LOW) ? 0 : 1;
    outb(0xa0 | ATA_DEV_IO_MOD_LBA | (master << 4) | (uint8_t)((lba >> 24) & 0xf),
        io_port + ATA_IO_RW_OFFSET_DRIVE_SELECT);
    ata_wait_register_400ns(ata_space.device_info_[dev].ctrl_port_);

    // real operation
    outb(cr, io_port + ATA_IO_RW_OFFSET_SECTOR_COUNT);
    outb((uint8_t)(lba & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_LOW);
    outb((uint8_t)((lba >> 8) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_MID);
    outb((uint8_t)((lba >> 16) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_HIGH);
    outb((uint8_t)cmd, io_port + ATA_IO_W_OFFSET_COMMAND);
    ata_wait_not_busy();

}

/**
 * @brief disable all IRQs
 */
void
ata_disable_irqs() {

    uint8_t origin_ctrl_master = inb(ATA_PRIMARY_PORT_CTRL_BASE),
        origin_ctrl_slave = inb(ATA_SECONDARY_PORT_CTRL_BASE);
    outb((origin_ctrl_master | ATA_DEV_CTRL_NIEN), ATA_PRIMARY_PORT_CTRL_BASE);
    outb((origin_ctrl_slave | ATA_DEV_CTRL_NIEN), ATA_SECONDARY_PORT_CTRL_BASE);
}

/**
 * @brief enable all IRQs
 */
void
ata_enable_irqs() {

    uint8_t origin_ctrl_master = inb(ATA_PRIMARY_PORT_CTRL_BASE),
        origin_ctrl_slave = inb(ATA_SECONDARY_PORT_CTRL_BASE);
    outb((origin_ctrl_master & ~ATA_DEV_CTRL_NIEN), ATA_PRIMARY_PORT_CTRL_BASE);
    outb((origin_ctrl_slave & ~ATA_DEV_CTRL_NIEN), ATA_SECONDARY_PORT_CTRL_BASE);
}

/**
 * @brief wait the status to be not busy but ready
 */
void
ata_wait_not_busy_but_ready() {
    port_t io_port =
        ata_space.device_info_[ata_space.current_select_].io_port_;

    // error checking
    while (
        (inb(io_port + ATA_IO_R_OFFSET_STATUS) & (ATA_STATUS_BSY | ATA_STATUS_RDY))
            != ATA_STATUS_RDY);
}

/**
 * @brief wait the status to be not busy but ready
 */
void
ata_wait_not_busy() {
    port_t io_port =
        ata_space.device_info_[ata_space.current_select_].io_port_;

    // wait not busy
    while ((inb(io_port + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_BSY));
}

/**
 * @brief detect ata device
 */
void
ata_space_init(void) {

    bzero(&ata_space, sizeof(ata_space_t));
    bzero(ata_devices, sizeof(ata_device_t) * ATA_MAX_SUPPORTED_DEVICES);
    ata_space.device_info_ = ata_devices;

    // disable IRQs during initialization
    ata_disable_irqs();

    port_t port_io = 0, port_ctrl = 0;
    size_t device_no = 0;
    // enumerate all buses
    for (size_t i = 0; i < ATA_TYPE_BUS_MAX; ++i) {
        port_io = i == 0 ?
            ATA_PRIMARY_PORT_IO_BASE : ATA_SECONDARY_PORT_IO_BASE;
        port_ctrl = i == 0 ?
            ATA_PRIMARY_PORT_CTRL_BASE : ATA_SECONDARY_PORT_CTRL_BASE;

        for (size_t j = 0; j < ATA_TYPE_BUS_WIRE_MAX; ++j) {
            ata_devices[ata_space.device_amount_].device_no_ = device_no++;
            ata_devices[ata_space.device_amount_].bus_       = i == 0 ?
                ATA_TYPE_BUS_PRIMARY : ATA_TYPE_BUS_SECONDARY;
            ata_devices[ata_space.device_amount_].bus_wire_  = j == 0 ?
                ATA_TYPE_BUS_WIRE_LOW : ATA_TYPE_BUS_WIRE_HIGH;
            ata_devices[ata_space.device_amount_].io_port_   = port_io;
            ata_devices[ata_space.device_amount_].ctrl_port_ = port_ctrl;

            // select device
            uint8_t flag = (j == 0) ? 0 : 1;
            outb(0xa0 | ATA_DEV_IO_MOD_LBA | (flag << 4),
                port_io + ATA_IO_RW_OFFSET_DRIVE_SELECT);
            ata_wait_register_400ns(port_ctrl);

            // send ata IDENTIFY command
            outb(ATA_CMD_IO_IDENTIFY, port_io + ATA_IO_W_OFFSET_COMMAND);
            ata_wait_register_400ns(port_ctrl);

            // polling status
            if (inb(port_io + ATA_IO_R_OFFSET_STATUS) == 0x00) {
                ata_devices[ata_space.device_amount_].valid_ = false;
                continue;
            } else {
                ata_devices[ata_space.device_amount_].valid_ = true;

                while (1) {
                    uint8_t status = inb(port_io + ATA_IO_R_OFFSET_STATUS);
                    if ((status & ATA_STATUS_ERR)) {
                        ata_devices[ata_space.device_amount_].device_type_ =
                            ATA_TYPE_DEVICE_UNKNOWN;
                        break;
                    }
                    if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRQ)) {
                        ata_devices[ata_space.device_amount_].device_type_ =
                            ATA_TYPE_DEVICE_ATA;
                        break;
                    }
                }

                ++ata_space.device_amount_;                 // increase after founded
            }

        } // end for(j)
    } // end for(i)

    ata_enable_irqs();

}
