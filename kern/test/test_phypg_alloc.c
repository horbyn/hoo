/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/driver/io.h"
#include "kern/module/mem.h"

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
    kprintf(">> 4th page: %x...then release\n\n", another_p1);
}

/**
 * @brief allocate physical pages continuous
 * (panic if there has no memory)
 */
static void
alloc_continuous() {
    kprintf("> TEST_PHYPG_ALLOC: continuous allocation <\n");

    kprintf(">> 100 pages: ");
    void *beg = 0, *end = 0;
    uint32_t max = 100;
    for (uint32_t i = 0; i < max; i++) {
        void *p = phy_alloc_page();
        if (i == 0) {
            beg = p;
            kprintf("from 0x%x ", beg);
        }
        if (i == max - 1) {
            end = p;
            kprintf("to 0x%x\n", end);
        }
    }

    for (uint32_t i = 0; i < max; ++i)
        phy_release_page((void *)((uint32_t)beg + i * PGSIZE));
    kprintf("\n\n");
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
