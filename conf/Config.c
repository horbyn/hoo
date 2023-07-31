/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "Config.h"

__attribute__ ((section(".config"))) static
    Gdt_t __gdt[SIZE_GDT];
__attribute__ ((section(".config"))) static
    Gdtr_t __gdtr;
__attribute__ ((section(".config"))) static
    ppg_range_t __kphymm;
__attribute__ ((section(".config"))) static
    ppg_t __kfree_list;
__attribute__ ((section(".page"))) static
    pgelem_t __page_dir[PGDIR_SIZE];
__attribute__ ((section(".page"))) static
    pgelem_t __page_tbl_4mb[PGDIR_SIZE];                    // only the low 4mb map

/**
 * @brief prehandling before entering kernel:
 * 
 * 1. load gdt; 2. construct physical memory linked-list; 3. paging
 */
void
kernel_config(void) {
    config_lgdt();
    config_free_list();
    config_paging();
}

/**
 * @brief load gdt
 */
void
config_lgdt(void) {
    set_gdt(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);         // #0 null
    set_gdt(1, 0xfffff, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1);   // #1 kernel code
    set_gdt(2, 0xfffff, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1);   // #2 kernel data
    set_gdt(3, 0xfffff, 0, 0, 1, 0, 1, 1, 3, 1, 0, 1, 1);   // #3 user code
    set_gdt(4, 0xfffff, 0, 0, 1, 0, 0, 1, 3, 1, 0, 1, 1);   // #4 user data

    set_gdtr();
}

/**
 * @brief setup physical memory
 */
void
config_free_list(void) {
    // initialize the management
    __kphymm.ppg_base = __kphymm.ppg_end = (uint8_t *)MM_BASE;
    __kphymm.pg_amount = 0;

    // travesal ARDS to find out the available mm.
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (size_t i = 0; i < *ards_num; ++i) {
        if (((ards + i)->type_ == ardstype_os)
        && ((ards + i)->base_low_ >= MM_BASE)) {
            uint32_t pages = (ards + i)->length_low_ / PGSIZE;
            __kphymm.pg_amount = pages;
            // use 8-bit to represent a page
            __kphymm.ppg_end += pages * 8;
        }
    }

    // NOTE:
    // from `MM_BASE` to `__kphymm.ppg_end` is used for management struct(i.e. `ppg_t`)
    // from `__kphymm.ppg_end` to maybe 0xffff_ffff is the real page available
    // initialize the `ppg_t` object
    ppg_t *cur = (ppg_t *)MM_BASE;
    ppg_t *worker = &__kfree_list;
    uint8_t *curpg = (uint8_t *)PGUP((uint32_t)__kphymm.ppg_end, PGSIZE);
    // we must skip some pages used for management struct
    size_t skip = ((uint32_t)curpg - (uint32_t)cur) / PGSIZE;

    // set the linked list for all the physical mm.
    for (size_t i = skip; i < __kphymm.pg_amount; ++i, curpg += PGSIZE) {
        cur[i].pgaddr = curpg;
        worker->next = cur + i;
        worker = cur + i;
    }
}

/**
 * @brief paging
 */
void
config_paging(void) {
    // setup low 4mb mapping
    create_pgtbl_map(__page_tbl_4mb, 0, 0, MB4 / PGSIZE);

    // setup kernel page dir table
    create_ptdir_map(__page_dir, 0, __page_tbl_4mb);        // #0
    create_ptdir_map(__page_dir, PD_INDEX(KERN_HIGH_MAPPING),
        __page_tbl_4mb);                                    // #768
    create_ptdir_map(__page_dir, PD_INDEX(0xffc00000),
        __page_dir);                                        // #1023

    // paging
    __asm__ ("movl %0, %%cr3" : :"r"(__page_dir));
    __asm__ ("\r\n"
        "movl %cr0,       %eax\r\n"
        "orl $0x80000000, %eax\r\n"
        "movl %eax,       %cr0");
}

/**
 * @brief Set the gdt descriptor
 * 
 * @param idx   the table idx(if beyond then nothing happen)
 * @param limit the maximun of addressing, which units are either 1B or 4KB
 * @param base  segment base linear address
 * @param a     best left clear
 * @param rw    readable or writable
 * @param dc    direction or conforming
 * @param exe   executable
 * @param sys   if system segment?
 * @param dpl   privilege
 * @param ps    if present in memory?
 * @param l     if the long mode?
 * @param db    if the protected mode?
 * @param g     granularity
 */
void
set_gdt(size_t idx, uint32_t limit, uint32_t base, uint8_t a,
uint8_t rw, uint8_t dc, uint8_t exe, uint8_t sys, uint8_t dpl,
uint8_t ps, uint8_t l, uint8_t db, uint8_t g) {
    ASSERT(idx >= SIZE_GDT);

    __gdt[idx].limit_15_0_        = (uint16_t)limit;
    __gdt[idx].limit_19_16_       = (uint8_t)(limit >> 16);
    __gdt[idx].base_15_0_         = (uint16_t)base;
    __gdt[idx].base_23_16_        = (uint8_t)(base >> 16);
    __gdt[idx].base_31_24_        = (uint8_t)(base >> 24);
    __gdt[idx].access_bytes_.a_   = a;
    __gdt[idx].access_bytes_.rw_  = rw;
    __gdt[idx].access_bytes_.dc_  = dc;
    __gdt[idx].access_bytes_.e_   = exe;
    __gdt[idx].access_bytes_.sys_ = sys;
    __gdt[idx].access_bytes_.dpl_ = dpl;
    __gdt[idx].access_bytes_.ps_  = ps;
    __gdt[idx].rsv_               = 0;
    __gdt[idx].long_              = l;
    __gdt[idx].db_                = db;
    __gdt[idx].g_                 = g;
}

/**
 * @brief Set the gdtr object
 */
void set_gdtr() {
    __gdtr.size_   = sizeof(__gdt);
    __gdtr.linear_ = __gdt;

    __asm__ ("lgdt (%0)" : :"r"(&__gdtr));
}

/**
 * @brief setup page table mapping
 * 
 * @param pgt               page table addr
 * @param ent_base          page table entry base
 * @param pg_phy_addr_base  the physical address base of the page
 * @param n                 the amount needed to setup
 */
void
create_pgtbl_map(void *pgt, size_t ent_base, void *
pg_phy_addr_base, size_t n) {
    // DONOT handle the following condition temporary
    ASSERT(pgt == null);
    ASSERT(ent_base < 0 || ent_base > PGDIR_SIZE);
    ASSERT(n > PGTBL_SIZE);

    if (n == 0)    return;

    // adjust the amount to setup
    n = (ent_base + n) > PGTBL_SIZE ?
        PGTBL_SIZE - ent_base : n;

    pgelem_t *worker = (pgelem_t *)pgt;
    uint32_t pg =
        (uint32_t)PGUP((uint32_t)pg_phy_addr_base, PGSIZE);

    for (size_t i = 0; i < n; ++i, pg += PGSIZE)
        worker[ent_base + i] =
            ((uint32_t)pg | PGENT_US | PGENT_RW | PGENT_PS);
}

/**
 * @brief setup page dir mapping
 * 
 * @param pgd page dir addr
 * @param ent page dir entry
 * @param pgt page table physical addr
 */
void
create_ptdir_map(void *pgd, size_t ent, void *pgt) {
    ASSERT(pgd == null);
    ASSERT(ent < 0 || ent > PGDIR_SIZE);

    if (pgt == null)    return;

    pgelem_t *worker = (pgelem_t *)pgd;
    worker[ent] =
        ((uint32_t)PGUP((uint32_t)pgt, PGSIZE) | PGENT_US 
        | PGENT_RW | PGENT_PS);
}

