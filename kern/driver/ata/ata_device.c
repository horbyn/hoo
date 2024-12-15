/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_device.h"
#include "kern/panic.h"
#include "user/lib.h"

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
    ataspc_t *ata_space = get_ataspace();
    if (ata_space->device_amount_ != 0 && dev_no >= ata_space->device_amount_)
        panic("ata_check_invalid(): device invalid");
}

/**
 * @brief wait the status to be not busy but ready
 */
static void
ata_wait() {
    ataspc_t *ata_space = get_ataspace();
    uint16_t io_port =
        ata_space->device_info_[ata_space->current_select_].io_port_;

    // wait not busy
    while ((inb(io_port + ATA_IO_R_OFFSET_STATUS)
        & (ATA_STATUS_RDY | ATA_STATUS_BSY)) != ATA_STATUS_RDY);
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
atabuff_set(atabuff_t *ibuff, void *buff, uint32_t len, int lba, atacmd_t cmd) {
    if (ibuff == null)    panic("atabuff_set(): null pointer");
    if (buff == null)    ibuff->buff_ = null;
    else    ibuff->buff_ = buff;
    ibuff->len_ = len;
    ibuff->lba_ = lba;
    ibuff->cmd_ = cmd;
    ibuff->finish_ = false;
}

/**
 * @brief get the ata space object
 * 
 * @return ata space object
 */
ataspc_t *
get_ataspace(void) {
    static ataspc_t __ata_space;
    return &__ata_space;
}

/**
 * @brief get the ata space object
 * 
 * @return ata space object
 */
atadev_t *
get_atadevice(void) {
    static atadev_t __ata_devices[ATA_MAX_SUPPORTED_DEVICES];
    return __ata_devices;
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
ata_set_cmd(uint32_t dev, uint32_t lba, uint8_t cr, atacmd_t cmd) {

    ataspc_t *ata_space = get_ataspace();
    atadev_t *ata_devices = get_atadevice();
    uint16_t io_port = ata_space->device_info_[dev].io_port_;

    ata_check_invalid(dev);

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

    ata_wait();
}

/**
 * @brief detect ata device
 */
void
ata_detect(void) {

    ataspc_t *space = get_ataspace();
    bzero(space, sizeof(ataspc_t));
    atadev_t *dev = get_atadevice();
    bzero(dev, sizeof(atadev_t) * ATA_MAX_SUPPORTED_DEVICES);
    space->device_info_ = dev;

    uint16_t port_io = 0, port_ctrl = 0;
    uint32_t device_no = 0;
    atadev_t *worker = dev + space->device_amount_;
    // enumerate all buses
    for (uint32_t i = 0; i < ATA_TYPE_BUS_MAX; ++i) {
        port_io = i == 0 ?
            ATA_PRIMARY_PORT_IO_BASE : ATA_SECONDARY_PORT_IO_BASE;
        port_ctrl = i == 0 ?
            ATA_PRIMARY_PORT_CTRL_BASE : ATA_SECONDARY_PORT_CTRL_BASE;

        for (uint32_t j = 0; j < ATA_TYPE_BUS_WIRE_MAX; ++j) {
            worker->device_no_ = device_no++;
            worker->bus_       =
                i == 0 ? ATA_TYPE_BUS_PRIMARY : ATA_TYPE_BUS_SECONDARY;
            worker->bus_wire_  =
                j == 0 ? ATA_TYPE_BUS_WIRE_LOW : ATA_TYPE_BUS_WIRE_HIGH;
            worker->io_port_   = port_io;
            worker->ctrl_port_ = port_ctrl;

            // select device
            uint8_t flag = (j == 0) ? 0 : 1;
            outb(0xa0 | ATA_DEV_IO_MOD_LBA | (flag << 4),
                port_io + ATA_IO_RW_OFFSET_DRIVE_SELECT);
            ata_wait_register_400ns(port_ctrl);

            // send ata IDENTIFY command
            outb(ATA_CMD_IO_IDENTIFY, port_io + ATA_IO_W_OFFSET_COMMAND);
            ata_wait_register_400ns(port_ctrl);

            // whether the device is exist
            if (inb(port_io + ATA_IO_R_OFFSET_STATUS) == 0x00) {
                worker->valid_ = false;
            } else {
                // polling status
                while (inb(port_io + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_BSY);

                // some ATAPI devices do not follow the spec, checks them
                if (inb(port_io + ATA_IO_RW_OFFSET_LBA_MID)
                    && inb(port_io + ATA_IO_RW_OFFSET_LBA_HIGH)) {
                    worker->device_type_ = ATA_TYPE_DEVICE_UNKNOWN;
                    worker->valid_ = false;
                } else {
                    // wait for device ready
                    while (!((inb(port_io + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_DRQ)
                        || (inb(port_io + ATA_IO_R_OFFSET_STATUS) & ATA_STATUS_ERR)));

                    worker->valid_ = true;
                    static ataid_t id;
                    insw(&id, sizeof(ataid_t), port_io + ATA_IO_RW_OFFSET_DATA);
                    worker->device_type_ = ata_get_device_type(&id);
                    worker->total_sectors_ = ata_get_sectors(&id);
                    ata_get_serial_number(&id, &worker->dev_serial_);
                    ata_get_model_number(&id, &worker->dev_model_);

                    // increase after founded
                    ++space->device_amount_;
                } // end if-else(dev not follow spec)
            } // end if-else(dev not exist)
            ++worker;
        } // end for(j)
    } // end for(i)

}
