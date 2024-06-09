/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata.h"

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
atabuff_set(atabuff_t *ibuff, void *buff, uint32_t len, uint32_t lba,
ata_cmd_t cmd) {

    ibuff->buff_ = buff;
    ibuff->len_ = len;
    ibuff->lba_ = lba;
    ibuff->cmd_ = cmd;
    ibuff->thread_ = get_current_pcb()->tid_;
}

/**
 * @brief get the ata space object
 * 
 * @return ata space object
 */
ata_space_t *
get_ataspace(void) {
    static ata_space_t ata_space;
    return &ata_space;
}

/**
 * @brief get the ata space object
 * 
 * @return ata space object
 */
ata_device_t *
get_atadevice(void) {
    static ata_device_t ata_devices[ATA_MAX_SUPPORTED_DEVICES];
    return ata_devices;
}

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
    ata_space_t *ata_space = get_ataspace();
    if (ata_space->device_amount_ != 0 && dev_no >= ata_space->device_amount_)
        panic("ata_check_invalid(): device invalid");
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
static void
ata_enable_irqs() {

    uint8_t origin_ctrl_master = inb(ATA_PRIMARY_PORT_CTRL_BASE),
        origin_ctrl_slave = inb(ATA_SECONDARY_PORT_CTRL_BASE);
    outb((origin_ctrl_master & ~ATA_DEV_CTRL_NIEN), ATA_PRIMARY_PORT_CTRL_BASE);
    outb((origin_ctrl_slave & ~ATA_DEV_CTRL_NIEN), ATA_SECONDARY_PORT_CTRL_BASE);
}

/**
 * @brief wait the status to be not busy but ready
 */
static void
ata_wait_not_busy() {
    ata_space_t *ata_space = get_ataspace();
    uint16_t io_port =
        ata_space->device_info_[ata_space->current_select_].io_port_;

    // wait not busy
    while ((inb(io_port + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_BSY));
}

/**
 * @brief write command to ide register
 * 
 * @param dev device no.
 * @param lba lba no.
 * @param cr  the sector amount
 * @param cmd commnd
 * @param is_irq true if method is IRQs
 */
void
ata_set_cmd(uint32_t dev, uint32_t lba, uint8_t cr, ata_cmd_t cmd, bool is_irq) {

    ata_space_t *ata_space = get_ataspace();
    ata_device_t *ata_devices = get_atadevice();

    ata_check_invalid(dev);

    uint16_t io_port =
        ata_space->device_info_[dev].io_port_;

    if (is_irq)    ata_enable_irqs();
    else    ata_disable_irqs();

    // select device
    uint8_t master =
        (ata_devices[dev].bus_wire_ == ATA_TYPE_BUS_WIRE_LOW) ? 0 : 1;
    outb(0xa0 | ATA_DEV_IO_MOD_LBA | (master << 4) | (uint8_t)((lba >> 24) & 0xf),
        io_port + ATA_IO_RW_OFFSET_DRIVE_SELECT);
    ata_wait_register_400ns(ata_space->device_info_[dev].ctrl_port_);

    // real operation
    outb(cr, io_port + ATA_IO_RW_OFFSET_SECTOR_COUNT);
    outb((uint8_t)(lba & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_LOW);
    outb((uint8_t)((lba >> 8) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_MID);
    outb((uint8_t)((lba >> 16) & 0xff), io_port + ATA_IO_RW_OFFSET_LBA_HIGH);
    outb((uint8_t)cmd, io_port + ATA_IO_W_OFFSET_COMMAND);
    ata_wait_not_busy();
}

/**
 * @brief detect ata device
 */
void
ata_space_init(void) {

    ata_space_t *ata_space = get_ataspace();
    bzero(ata_space, sizeof(ata_space_t));
    ata_device_t *ata_devices = get_atadevice();
    bzero(ata_devices, sizeof(ata_devices));
    ata_space->device_info_ = ata_devices;

    // disable IRQs during initialization
    ata_disable_irqs();

    uint16_t port_io = 0, port_ctrl = 0;
    uint32_t device_no = 0;
    // enumerate all buses
    for (uint32_t i = 0; i < ATA_TYPE_BUS_MAX; ++i) {
        port_io = i == 0 ?
            ATA_PRIMARY_PORT_IO_BASE : ATA_SECONDARY_PORT_IO_BASE;
        port_ctrl = i == 0 ?
            ATA_PRIMARY_PORT_CTRL_BASE : ATA_SECONDARY_PORT_CTRL_BASE;

        for (uint32_t j = 0; j < ATA_TYPE_BUS_WIRE_MAX; ++j) {
            ata_devices[ata_space->device_amount_].device_no_ = device_no++;
            ata_devices[ata_space->device_amount_].bus_       = i == 0 ?
                ATA_TYPE_BUS_PRIMARY : ATA_TYPE_BUS_SECONDARY;
            ata_devices[ata_space->device_amount_].bus_wire_  = j == 0 ?
                ATA_TYPE_BUS_WIRE_LOW : ATA_TYPE_BUS_WIRE_HIGH;
            ata_devices[ata_space->device_amount_].io_port_   = port_io;
            ata_devices[ata_space->device_amount_].ctrl_port_ = port_ctrl;

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
                ata_devices[ata_space->device_amount_].valid_ = false;
                continue;
            } else {
                ata_devices[ata_space->device_amount_].valid_ = true;

                static ata_identify_data_t identify;
                insw(&identify, sizeof(ata_identify_data_t),
                    port_io + ATA_IO_RW_OFFSET_DATA);
                ata_devices[ata_space->device_amount_].device_type_ =
                    ata_get_device_type(&identify);
                ata_devices[ata_space->device_amount_].total_sectors_ =
                    ata_get_sectors(&identify);
                ata_get_serial_number(&identify,
                    &ata_devices[ata_space->device_amount_].dev_serial_);
                ata_get_model_number(&identify,
                    &ata_devices[ata_space->device_amount_].dev_model_);

                // increase after founded
                ++ata_space->device_amount_;
            }

        } // end for(j)
    } // end for(i)

    ata_enable_irqs();

}
