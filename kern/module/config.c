/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "config.h"

static Desc_t __gdt[SIZE_GDT];
static Gdtr_t __gdtr;

/**
 * @brief paging
 */
static void
paging() {
    pgelem_t *pdir = get_idle_pgdir();
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;

    pdir[0] = (pgelem_t)((SEG_PGTABLE * 16) | flags);
    pdir[PD_INDEX(KERN_HIGH_MAPPING)] = (pgelem_t)((SEG_PGTABLE * 16) | flags);
    pdir[1023] = (pgelem_t)((uint32_t)pdir | flags);

    for (uint32_t va = 0, pa = 0, i = 0; pa < MM_BASE; va += PGSIZE, pa += PGSIZE, ++i)
        set_mapping(pdir, va, pa);

    // paging
    __asm__ ("movl %0, %%cr3" : :"r"(V2P(pdir)));
    __asm__ ("\r\n"
        "movl %cr0,       %eax\r\n"
        "orl $0x80000000, %eax\r\n"
        "movl %eax,       %cr0");
}

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
    set_gdt(&__gdt[5], sizeof(tss_t), (uint32_t)get_idle_tss(),
        1, 0, 0, 1, 0, 3, 1, 0, 1, 0);

    // load tr with seletor #5
    // note that the busy field of tss descriptor will always be set once
    // `ltr` its corresponding selector whatever this field set or clear
    __asm__ ("movw $((5 * 8) | 3), %ax\n\t"
        "ltr %ax");
}

/**
 * @brief kernel initialization module for beginning configurations
 */
void
kinit_config(void) {
    paging();
    config_gdt();
    config_tss();
}
