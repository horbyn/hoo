/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "format_list.h"
#include "pm.h"
#include "vm_kern.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief convert a page into multiple specific elements
 * 
 * @param page_va   the virtual address of a page
 * @param type_size specific type
 */
static void
fmtlist_init(fmtlist_t *page_va, uint32_t type_size) {
    if (page_va == null)    panic("fmtlist_init(): null pointer");

    uint8_t *p = (uint8_t *)(PGDOWN(page_va, PGSIZE));
    if ((uint32_t)p != (uint32_t)page_va)
        panic("fmtlist_init(): page size is not 4KB");
    fmtlist_t *fmtlist = (fmtlist_t *)p;
    fmtlist->type_size_ = type_size;
    fmtlist->first_off_ = sizeof(fmtlist_t);
    uint32_t elem_sz = sizeof(node_t) + type_size;
    fmtlist->capacity_ = (PGSIZE - fmtlist->first_off_) / elem_sz;
    fmtlist->next_ = null;

    /*
     * transform to the following formatting
     * ┌───────────┬────────┬──────────┬─────────┬────────┬──────────┬────┐
     * │ fmtlist_t │ node_t │   xx_t   │         │ node_t │   xx_t   │    │
     * │           │                   │   ...   │                   │ .. │
     * │  (head)   │      (elem 1)     │         │      (elem x)     │    │
     * └───────────┴────────┴──────────┴─────────┴────────┴──────────┴────┘
     */

    list_init(&fmtlist->list_, false);
    p += fmtlist->first_off_;
    for (uint32_t i = 0; i < fmtlist->capacity_; ++i) {
        node_t *n = (node_t *)p;
        uint8_t *type_elem = (uint8_t *)(p + sizeof(node_t));
        bzero(type_elem, type_size);
        p += elem_sz;
        node_set(n, type_elem, (node_t *)p);
        list_insert(&fmtlist->list_, n, LSIDX_AFTAIL(&fmtlist->list_));
    }
}

/**
 * @brief get a element from the formatting list
 * 
 * @param fmtlist formatting list
 * @param size    element size
 * @return the free element
 */
void *
fmtlist_alloc(fmtlist_t **fmtlist, uint32_t size) {
    if (fmtlist == null)    panic("fmtlist_alloc(): null pointer");

    void *ret = null;
    fmtlist_t *pre = null, *list = *fmtlist;
    while (list != null) {
        if (list_isempty(&list->list_) == false) {
            ret = list_remove(&list->list_, 1)->data_;
            break;
        }
        pre = list;
        list = list->next_;
    }
    if (ret == null) {
        // request new page
        void *pa = phy_alloc_page();
        void *va = vir_alloc_kern();
        set_mapping(va, pa, PGFLAG_US | PGFLAG_RW | PGFLAG_PS);
        fmtlist_init(va, size);

        if (pre == null)    *fmtlist = va;
        else    pre->next_ = va;

        ret = list_remove(&((fmtlist_t *)va)->list_, 1)->data_;
    }

    return ret;
}

/**
 * @brief reclaim the list elements of specific type (DO NOT VERIFY whether
 * the element is belong to this list)
 * 
 * @param fmtlist   the formatting list
 * @param elem      the free element
 * @param elem_size the length of the free element
 */
bool
fmtlist_release(fmtlist_t **fmtlist, void *elem, uint32_t elem_size) {

    bool release = false;
    if (elem == null)    return release;
    fmtlist_t *pre = null, *list = null;

    if (fmtlist != null)    list = *fmtlist;
    while (list != null) {
        if (elem_size != list->type_size_)
            panic("fmtlist_release(): element type not match");
        if ((void *)list != (void *)PGDOWN(elem, PGSIZE)) {
            pre = list;
            list = list->next_;
            continue;
        }

        uint8_t *p = (uint8_t *)elem;
        p -= sizeof(node_t);
        bzero(p, sizeof(node_t) + list->type_size_);
        ((node_t *)p)->data_ = elem;
        list_insert(&list->list_, (node_t *)p, 1);

        if (list->list_.size_ == list->capacity_) {
            if (pre != null)    pre->next_ = list->next_;
            vir_release_kern(list);
            *fmtlist = null;
        }
        release = true;
        break;
    } // end while()

    return release;
}
