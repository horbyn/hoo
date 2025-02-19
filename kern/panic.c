#include "panic.h"
#include "x86.h"
#include "kern/driver/cga/cga.h"
#include "kern/hoo/hoo.h"
#include "kern/module/io.h"
#include "kern/page/page_stuff.h"
#include "kern/intr/intr_stack.h"

/**
 * @brief 打印栈帧
 */
static void
trace() {
    kprintf("========== Stack trace ==========\n");

    uint32_t ebp = 0, ret = 0;
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));
    uint32_t border = PGUP(ebp, PGSIZE);

    for (uint32_t i = 0; ebp != 0 && ebp <= border; ++i) {
        ret = *(uint32_t *)(ebp + sizeof(uint32_t));

        for (uint32_t space = 0; space < i; ++space) {
            kprintf("  ");
        }
        kprintf("- 0x%x", ret);

        // 检查是否中断栈
        if (ret == (uint32_t)isr_part3) {
            kprintf(" *");
            istackcpu_t *istack = 
                (istackcpu_t *)(ebp + sizeof(uint32_t) * 2 + sizeof(istackos_t));
            kprintf(" <- 0x%x", istack->oldeip_);
        }

        kprintf("\n");
        ebp = *(uint32_t *)(ebp);
    }
}

/**
 * @brief 在系统 panic 时打印信息
 * 
 * @param extra 额外信息
 */
void
panic(const char *extra) {
    cga_clear();
    if (extra != null)
        kprintf("%s\n\n", extra);
    trace();
    disable_intr();
    hlt();
}
