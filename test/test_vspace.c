/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/driver/io.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"

/**
 * @brief virtual space test
 */
void
test_vspace() {
    clear_screen();
    kprintf(">          TEST_VSPACE          <\n");

    pcb_t *idle_pcb = get_current_pcb();
    tid_t idle_tid = pcb_get_tid(idle_pcb);

    // alloc 10 pages
    const uint32_t T1 = 10;
    void *va = vir_alloc_pages(idle_tid, &idle_pcb->vspace_, T1);
    for (uint32_t i = 0; i < T1; ++i) {
        void *pa = phy_alloc_page();
        set_mapping(idle_pcb->pdir_va_, (uint32_t)va + i * PGSIZE, (uint32_t)pa);
        kprintf("va: 0x%x --> pa: 0x%x\n", va + i * PGSIZE, pa);
    }

    vir_release_pages(idle_tid, &idle_pcb->vspace_, va);

}