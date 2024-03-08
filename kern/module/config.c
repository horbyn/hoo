/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "config.h"

__attribute__ ((section(".page"))) static pgelem_t __page_dir[PGDIR_SIZE];
__attribute__ ((section(".page"))) static pgelem_t __page_tbl_4mb[PGDIR_SIZE];  // only the low 4mb map
static Desc_t __gdt[SIZE_GDT];
static Gdtr_t __gdtr;
Tss_t __tss;

static void
paging() {
    // DO NOT call any functions as possible because the function addresses
    //     are incorrect before paging, unless specifying them
    pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;

    __page_dir[0] = (pgelem_t)__page_tbl_4mb | flags;
    __page_dir[PD_INDEX(KERN_HIGH_MAPPING)] = (pgelem_t)__page_tbl_4mb | flags;
    __page_dir[1023] = (pgelem_t)__page_dir | flags;

    for (uint32_t pa = 0, i = 0; pa < MB4; pa += PGSIZE, ++i)
        __page_tbl_4mb[i] = (pgelem_t)pa | flags;

    // paging
    __asm__ ("movl %0, %%cr3" : :"r"(__page_dir));
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
    set_gdt(&__gdt[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);         // #0 null
    set_gdt(&__gdt[1], 0xfffff, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1);   // #1 kernel code
    set_gdt(&__gdt[2], 0xfffff, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1);   // #2 kernel data
    set_gdt(&__gdt[3], 0xfffff, 0, 0, 1, 0, 1, 1, 3, 1, 0, 1, 1);   // #3 user code
    set_gdt(&__gdt[4], 0xfffff, 0, 0, 1, 0, 0, 1, 3, 1, 0, 1, 1);   // #4 user data

    set_gdtr(&__gdtr, __gdt, sizeof(__gdt));
}

/**
 * @brief setup tss
 */
static void
config_tss(void) {
    // #5 tss
    set_gdt(&__gdt[5], sizeof(Tss_t), (uint32_t)&__tss, 1, 0, 0, 1, 0, 3, 1, 0, 1, 0);

    // load tr with seletor #5
    // note that the busy field of tss descriptor will always be set once
    // `ltr` its corresponding selector whatever this field set or clear
    __asm__ ("movw $((5 * 8) | 3), %ax\n\t"
        "ltr %ax");
}

/**
 * @brief load gdt
 */
void
kernel_config(void) {
    paging();
    config_gdt();
    config_tss();
}
