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
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;

    pgelem_t *pdir = get_hoo_pgdir();
    pdir[0] = (pgelem_t)((SEG_PGTABLE * 16) | flags);
    pdir[PD_INDEX(KERN_HIGH_MAPPING)] = pdir[0];
    pdir[PG_STRUCT_SIZE - 1] = (pgelem_t)(V2P(pdir) | flags);

    pgelem_t *pg = (pgelem_t *)(SEG_PGTABLE * 16);
    for (uint32_t i = 0; i < (MM_BASE / PGSIZE); ++i)
        pg[i] = ((pgelem_t)(i * PGSIZE) | flags);

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
    set_gdt(&__gdt[5], sizeof(tss_t), (uint32_t)get_hoo_tss(),
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
