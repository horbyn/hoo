/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "mem.h"
#include "kern/x86.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/module/log.h"

static mminfo_t __mminfo = { MM_BASE, 0 };

/**
 * @brief get the memory info of the system
 */
static void
get_mminfo() {

    // traversal ARDS to find out the available mm.
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (uint32_t i = 0; i < *ards_num; ++i) {
        if ((ards[i].type_ == ARDS_TYPE_OS)
        && (ards[i].base_low_ == __mminfo.base_ + __mminfo.length_)) {
            // record only continuous memory space
            __mminfo.length_ += ards[i].length_low_;
        }
    }

    if (__mminfo.length_ == 0)
        panic("mem_info_init(): cannot get memory info");
}

/**
 * @brief memory initialization of the whole system
 * included physical and virtual portion
 */
void
kinit_memory() {
    get_mminfo();
    klog_write("[DEBUG] available memory: [0x%x, 0x%x)\n",
        __mminfo.base_, __mminfo.base_ + __mminfo.length_);
    init_phymm_system(__mminfo.length_);
    init_virmm_system();
}
