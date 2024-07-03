/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern.h"

/**
 * @brief kernel initialization
 */
void
kernel_init(void) {
    io_init();

#ifndef DEBUG
    kprintf("================ KERNEL IMAGE ================\n"
        "kern base: 0x%x,  kern end: 0x%x\n"
        "kern length: %dkb\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end,
        ((uint32_t)__kern_end - (uint32_t)__kern_base) / 1024);
#endif

    kinit_memory();
    kinit_config();
    kinit_tasks_system();
    kinit_isr_idt();
    kinit_dirver();
    kinit_fs();

#ifndef DEBUG
    ata_space_t *ata_space = get_ataspace();
    uint32_t select = ata_space->current_select_;
    kprintf("================ DEVICE  INFO ================"
        "\ndev no.:       %d"
        "\nserial no.:    %s"
        "\nmodel no.:     %s"
        "\ntype:          %s"
        "\ntotal sectors: %d\n\n",
        ata_space->device_info_[select].device_no_,
        (char *)&(ata_space->device_info_[select].dev_serial_),
        (char *)&(ata_space->device_info_[select].dev_model_),
        ENUM2STR_ATA_TYPE_DEVICE(
            ata_space->device_info_[select].device_type_),
        ata_space->device_info_[select].total_sectors_);

    kprintf("================ FILE  SYSTEM ================"
        "\nlba:"
        "\n    super block: %d"
        "\n    inode map:   %d"
        "\n    inodes:      %d"
        "\n    free map:    %d"
        "\n    free:        %d\n\n",
        __super_block.lba_super_block_, __super_block.lba_map_inode_,
        __super_block.lba_inodes_, __super_block.lba_map_free_,
        __super_block.lba_free_);
#endif

}

/**
 * @brief kernel run
 */
void
kernel_exec(void) {
    idle_init();

    // "hoo" sleeps until "idle" completes its initialization
    sleeplock_t *hoo_slplock = get_hoo_sleeplock();
    wait(&hoo_slplock->guard_);
    sleep(hoo_slplock);
    signal(&hoo_slplock->guard_);
    enable_intr();

#ifdef TEST
    test_phypg_alloc();
    test_vspace();
    test_schedule();
    test_disk_read();
#endif

#ifdef DEBUG
    clear_screen();
    debug_print_tasks();
#endif

#ifdef TEST
    test_fs();
#endif
}
