#include "mem.h"
#include "kern/x86.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/module/log.h"

static mminfo_t __mminfo = { MM_BASE, 0 };

/**
 * @brief 获取系统的内存
 */
static void
get_mminfo() {

    // 遍历 ARDS 找出可用内存
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (uint32_t i = 0; i < *ards_num; ++i) {
        if ((ards[i].type_ == ARDS_TYPE_OS)
        && (ards[i].base_low_ == __mminfo.base_ + __mminfo.length_)) {
            // 仅记录连续的内存空间
            __mminfo.length_ += ards[i].length_low_;
        }
    }

    if (__mminfo.length_ == 0)
        panic("get_mminfo(): cannot get memory info");
}

/**
 * @brief 整个系统的内存初始化，包括物理内存管理模块和虚拟内存管理模块
 */
void
kinit_memory() {
    get_mminfo();
    klog_write("[DEBUG] available memory: [0x%x, 0x%x)\n",
        __mminfo.base_, __mminfo.base_ + __mminfo.length_);
    init_phymm_system(__mminfo.length_);
    init_virmm_system();
}
