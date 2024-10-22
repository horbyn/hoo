/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "dynamic.h"

/**
 * @brief heap memory allocation
 * 
 * @param size size to allocated
 * @return memory
 */
void *
dyn_alloc(uint32_t size) {
    if (size == 0)    return null;

    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;

    while (mngr != null) {
        if (mngr->size_ >= size) {
            break;
        }
        mngr = mngr->next_;
    }
    if (mngr == null) {
        uint32_t pages = size <= PGSIZE ? 1 : (size + PGSIZE - 1) / PGSIZE;

        void *va = vir_alloc_pages(&cur_pcb->vmngr_, pages, cur_pcb->break_);
        for (uint32_t i = 0; i < pages; ++i) {
            void *pa = phy_alloc_page();
            set_mapping(va + i * PGSIZE, pa, PGFLAG_US | PGFLAG_RW | PGFLAG_PS);
        }
        return va;
    }

    return fmtlist_alloc(&mngr->chain_, mngr->size_);
}

/**
 * @brief heap memory releasing
 * 
 * @param ptr memory address (virtual address)
 */
void
dyn_free(void *ptr) {
    if (ptr == null)    return;

    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;

    while (mngr != null) {
        if (fmtlist_release(&mngr->chain_, ptr, mngr->size_))    break;
        mngr = mngr->next_;
    }
    if (mngr == null)    vir_release_pages(&cur_pcb->vmngr_, ptr, true);
}
