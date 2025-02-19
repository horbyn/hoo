#include "dynamic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "user/lib.h"

/**
 * @brief 堆内存分配
 * 
 * @param size 要分配的大小
 * @return 可用的虚拟地址
 */
void *
dyn_alloc(uint32_t size) {
    if (size == 0)    return null;

    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;
    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    while (mngr != null) {
        if (mngr->size_ >= size) {
            if (mngr->chain_ == null) {
                void *pa = phy_alloc_page();
                mngr->chain_ = vir_alloc_pages(&cur_pcb->vmngr_, 1, cur_pcb->break_);
                set_mapping(mngr->chain_, pa, flags);
                bzero(mngr->chain_, PGSIZE);
            }
            break;
        }
        mngr = mngr->next_;
    }
    if (mngr == null) {
        uint32_t pages = size <= PGSIZE ? 1 : (size + PGSIZE - 1) / PGSIZE;

        void *va = vir_alloc_pages(&cur_pcb->vmngr_, pages, cur_pcb->break_);
        for (uint32_t i = 0; i < pages; ++i) {
            void *pa = phy_alloc_page();
            set_mapping(va + i * PGSIZE, pa, flags);
        }
        return va;
    }

    return fmtlist_alloc(&mngr->chain_, mngr->size_);
}

/**
 * @brief 堆内存释放
 * 
 * @param ptr 内存虚拟地址
 */
void
dyn_free(void *ptr) {
    if (ptr == null)    return;

    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;

    while (mngr != null) {
        if (fmtlist_release(&mngr->chain_, ptr, mngr->size_)) {
            if (&mngr->chain_ != null &&
                mngr->chain_->capacity_ == mngr->chain_->list_.size_) {

                vir_release_pages(&cur_pcb->vmngr_, (void *)PGDOWN(ptr, PGSIZE), true);
                mngr->chain_ = null;
            }
            break;
        }
        mngr = mngr->next_;
    }
    if (mngr == null)    vir_release_pages(&cur_pcb->vmngr_, ptr, true);
}
