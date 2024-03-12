/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern/mem/mm.h"
#include "kern/driver/io.h"

static void
alloc_random() {
    kprintf("> TEST_PHYPG_ALLOC: random allocation <\n");

    void *p1 = phy_alloc_page();
    phy_release_page(p1);
    kprintf(">> 1st page: %x...then release\n", p1);
    void *p2 = phy_alloc_page();
    phy_release_page(p2);
    kprintf(">> 2nd page: %x...then release\n", p2);

    void *p = null;
    for (uint32_t i = 0; i < 10; ++i) {
        p = phy_alloc_page();
        phy_release_page(p);
    }
    void *p13 = phy_alloc_page();
    phy_release_page(p13);
    kprintf(">> 3rd page: %x...then release\n", p13);

    const mminfo_t *info = mem_info_get();
    uint32_t page_cr = (info->length_ >> 12) - 13;
    for (uint32_t i = 0; i < page_cr; ++i) {
        p = phy_alloc_page();
        phy_release_page(p);
    }
    void *another_p1 = phy_alloc_page();
    phy_release_page(another_p1);
    kprintf(">> 4th page: %x...then release\n", another_p1);
}

/**
 * @brief allocate physical pages continuous
 * (panic if there has no memory)
 */
static void
alloc_continuous() {
    kprintf("> TEST_PHYPG_ALLOC: continuous allocation <\n");

    kprintf(">> 100 pages: ");
    void *beg100 = 0, *end100 = 0;
    uint32_t max100 = 100;
    for (uint32_t i = 0; i < max100; i++) {
        void *p = phy_alloc_page();
        if (i == 0) {
            beg100 = p;
            kprintf("from 0x%x ", beg100);
        }
        if (i == max100 - 1) {
            end100 = p;
            kprintf("to 0x%x\n", end100);
        }
    }

    kprintf(">> 10000 pages: ");
    void *beg10000 = 0, *end10000 = 0;
    uint32_t max10000 = 10000;
    for (uint32_t i = 0; i < max10000; i++) {
        void *p = phy_alloc_page();
        if (i == 0) {
            beg10000 = p;
            kprintf("from 0x%x ", beg10000);
        }
        if (i == max10000 - 1) {
            end10000 = p;
            kprintf("to 0x%x\n", end10000);
        }
    }

    for (uint32_t i = 0; i < max100 + max10000; ++i)
        phy_release_page((void *)((uint32_t)beg100 + i * PGSIZE));
}

/**
 * @brief test physical pages allocation
 */
void
test_phypg_alloc(void) {
    clear_screen();
    alloc_random();
    alloc_continuous();
}
