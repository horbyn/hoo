/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/module/driver.h"

static uint8_t buff[BYTES_SECTOR];

/**
 * @brief irq method
 */
static void
disk_rw_with_irq() {
    kprintf("> TEST_ATA_IO_IRQ <\n");

    ata_driver_init(ATA_METHOD_IRQ);

    bzero(buff, sizeof(buff));
    for (uint32_t i = 0; i < sizeof(buff); ++i)
        buff[i] = (uint8_t)i;

    // write data to lba-0 of disk
    ata_driver_rw(buff, sizeof(buff), 0, ATA_CMD_IO_WRITE);

    // read lba-0 from disk to buff
    bzero(buff, sizeof(buff));
    ata_driver_rw(buff, sizeof(buff), 0, ATA_CMD_IO_READ);

    for (uint32_t i = 0; i < sizeof(buff); ++i)
        kprintf("%x ", buff[i]);

    kprintf("\n");
}

/**
 * @brief polling method
 */
static void
disk_rw_with_polling() {
    kprintf("> TEST_ATA_IO_POLLING <\n");

    ata_driver_init(ATA_METHOD_POLLING);

    // read lba-0 from disk to buff
    bzero(buff, sizeof(buff));
    ata_driver_rw(buff, sizeof(buff), 0, ATA_CMD_IO_READ);

    for (uint32_t i = 0; i < sizeof(buff); ++i)
        kprintf("%x ", buff[i]);

    bzero(buff, sizeof(buff));
    for (uint32_t i = 0; i < sizeof(buff); ++i)
        buff[i] = (uint8_t)i;

    // write data to lba-0 of disk
    ata_driver_rw(buff, sizeof(buff), 0, ATA_CMD_IO_WRITE);

    kprintf("\n");
}

/**
 * @brief test the reading from disk
 */
void
test_disk_read() {

    // backup
    static uint8_t backup_buff[BYTES_SECTOR];
    bzero(backup_buff, sizeof(backup_buff));
    ata_driver_rw(backup_buff, sizeof(backup_buff), 0, ATA_CMD_IO_READ);

    clear_screen();
    disk_rw_with_polling();
    clear_screen();
    disk_rw_with_irq();

    // restore
    ata_driver_rw(backup_buff, sizeof(backup_buff), 0, ATA_CMD_IO_WRITE);
    ata_driver_init(ATA_METHOD_IRQ); // restore the IRQ method
}
