/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "debug.h"

/**
 * @brief display some messages in collapse
 * 
 * @param msg 
 */
void
panic(const char *msg) {
#ifdef DEBUG
    if (msg)    kprintf(msg);
#else
    (void)msg;
#endif

    trace(null);
    hlt();
}

/**
 * @brief print the stack frame so far
 * 
 * @param extra some extra info
 */
void
trace(const char *extra) {
    clear_screen();
    if (extra)
        kprintf("%s\n\n", extra);
    kprintf("========== Stack trace ==========\n");

    uint32_t ebp = 0, ret = 0;
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));

    for (size_t i = 0; ebp != 0; ++i) {
        for (size_t space = 0; space < i; ++space) {
            kprintf("  ");
        }
        kprintf("- ");

        ret = *(uint32_t *)(ebp + sizeof(uint32_t));
        kprintf("0x%x", ret);

        // check whether it is interrupt stack
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
