/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "conf.h"
#include "kern/module/log.h"

static Desc_t __gdt[SIZE_GDT];
static Gdtr_t __gdtr;

/**
 * @brief setup gdt
 */
static void
config_gdt(void) {
    // #0 null
    set_gdt(&__gdt[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // #1 kernel code
    set_gdt(&__gdt[1], 0xfffff, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1);
    // #2 kernel data
    set_gdt(&__gdt[2], 0xfffff, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1);
    // #3 user code
    set_gdt(&__gdt[3], 0xfffff, 0, 0, 1, 0, 1, 1, 3, 1, 0, 1, 1);
    // #4 user data
    set_gdt(&__gdt[4], 0xfffff, 0, 0, 1, 0, 0, 1, 3, 1, 0, 1, 1);

    set_gdtr(&__gdtr, __gdt, sizeof(__gdt));
}

/**
 * @brief setup tss
 */
static void
config_tss(void) {
    // #5 tss
    set_gdt(&__gdt[5], sizeof(tss_t), (uint32_t)get_hoo_tss(),
        1, 0, 0, 1, 0, 3, 1, 0, 1, 0);

    // load tr with seletor #5
    // note that the busy field of tss descriptor will always be set once
    // `ltr` its corresponding selector whatever this field set or clear
    void *tr_value = null;
    __asm__ ("movw $((5 * 8) | 3), %%ax\n\t"
        "ltr %%ax\n\t"
        "str %0" : "=a"(tr_value) :);
    klog_write("[DEBUG] tr: 0x%x\n", tr_value);
}

/**
 * @brief kernel configuration
 */
void
kinit_config(void) {
    klog_write("[DEBUG] null pointer: 0x%x\n", null);
    paging();
    config_gdt();
    config_tss();
}
