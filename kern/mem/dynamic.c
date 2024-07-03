/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "dynamic.h"

/**
 * @brief array list formatting
 * 
 * @param list array list
 * @param type_size single element size
 */
static void
arrlist_init(arrlist_t *list, uint32_t type_size) {
    if (list == null)    panic("arrlist_init(): null pointer");

    uint8_t *p = (uint8_t *)(PGDOWN((uint32_t)list, PGSIZE));
    if ((uint32_t)p != (uint32_t)list)
        panic("arrlist_init(): page size is not 4KB");
    arrlist_t *arrlist = (arrlist_t *)p;
    arrlist->type_size_ = type_size;
    arrlist->first_off_ = sizeof(arrlist_t);
    *((void **)(&arrlist->head_)) = (void *)(p + arrlist->first_off_);
    arrlist->capacity_ = (PGSIZE - arrlist->first_off_) / type_size;
    arrlist->size_ = arrlist->capacity_;
    arrlist->next_ = null;

    /*
     * transform to the following formatting
     * ┌───────────┬────────────┬─────────┬────────────┬────┐
     * │ arrlist_t │            │         │            │    │
     * │           │            │   ...   │            │ .. │
     * │  (head)   │  (elem 1)  │         │  (elem x)  │    │
     * └───────────┴────────────┴─────────┴────────────┴────┘
     */

    p += arrlist->first_off_;
    for (uint32_t i = 0; i < arrlist->capacity_; ++i, p += type_size) {
        if (i == arrlist->capacity_ - 1)    *((void **)p) = null;
        else    *((void **)p) = (void *)(p + type_size);
    }
}

/**
 * @brief arrlist pop
 * 
 * @param list array list
 * @return node
 */
static void *
arrlist_pop(arrlist_t *list) {
    void *ret = list->head_;
    list->head_ = *((void **)list->head_);
    --list->size_;
    return ret;
}

/**
 * @brief get a free element from the array list
 * 
 * @param list array list
 * @param size element size
 * @return the free element
 */
static void *
arrlist_alloc(arrlist_t *list, uint32_t size) {

    void *ret = null;
    arrlist_t *pre = null;
    while (list) {
        if (list->size_ != 0) {
            ret = arrlist_pop(list->head_);
            break;
        }
        pre = list;
        list = list->next_;
    }
    if (ret == null) {
        // request new page
        pcb_t *cur_pcb = get_current_pcb();
        void *pa = phy_alloc_page();
        void *va = vir_alloc_pages(cur_pcb, 1);
        set_mapping(&cur_pcb->pgstruct_, (uint32_t)va, (uint32_t)pa,
            PGENT_US | PGENT_RW | PGENT_PS);
        arrlist_init(va, size);// not null, 0x20

        if (pre == null)    list = va;
        else    pre->next_ = va;

        ret = arrlist_pop(list->head_);
    }

    return ret;
}

/**
 * @brief reclaim the list elements of specific type (DO NOT VERIFY whether
 * the element is belong to this list)
 * 
 * @param fmtlist   the array list
 * @param elem      the free element
 * @param elem_size the length of the free element
 * 
 * @retval true:  release succeeded
 * @retval false: release failed
 */
static bool
arrlist_release(arrlist_t *list, void *elem, uint32_t elem_size) {
    if (list == null)    panic("arrlist_release(): null pointer");
    if (elem_size != list->type_size_)
        panic("arrlist_release(): element type not match");

    arrlist_t *pre = null;
    while (list) {
        if ((void *)list == (void *)PGDOWN((uint32_t)elem, PGSIZE)) {
            void *temp = list->head_;
            list->head_ = elem;
            *((void **)elem) = temp;
            ++list->size_;

            if (list->size_ == list->capacity_) {
                if (pre)
                    pre->next_ = list->next_;

                phy_release_vpage(get_current_pcb(), list);
                list = null;
            }

            break;
        }
        pre = list; 
        list = list->next_;
    } // end while()

    return list ? true : false;
}

/**
 * @brief heap memory allocation
 * 
 * @param size size to allocated
 * @return memory
 */
void *
dyn_alloc(uint32_t size) {
    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;

    while (mngr) {
        if (mngr->size_ >= size) {
            break;
        }
        mngr = mngr->next_;
    }
    if (mngr == null) {
        uint32_t pages = size <= PGSIZE ? 1 : (size + PGSIZE - 1) / PGSIZE;

        void *va = vir_alloc_pages(cur_pcb, pages);
        for (uint32_t i = 0; i < pages; ++i) {
            void *pa = phy_alloc_page();
            set_mapping(&cur_pcb->pgstruct_, (uint32_t)va + i * PGSIZE,
                (uint32_t)pa, PGENT_US | PGENT_RW | PGENT_PS);
        }
        return va;
    }

    return arrlist_alloc(mngr->chain_, mngr->size_);
}

/**
 * @brief heap memory releasing
 * 
 * @param ptr memory address (virtual address)
 */
void
dyn_free(void *ptr) {
    pcb_t *cur_pcb = get_current_pcb();
    buckx_mngr_t *mngr = cur_pcb->hmngr_;

    while (mngr) {
        if (arrlist_release(mngr->chain_, ptr, mngr->size_))    break;
        mngr = mngr->next_;
    }
    // if (mngr == null)    phy_release_vpage(cur_pcb, ptr); // single page not multiple
    if (mngr == null)    vir_release_pages(cur_pcb, ptr);
}
