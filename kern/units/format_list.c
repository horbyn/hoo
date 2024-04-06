/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "format_list.h"

/**
 * @brief convert a page into multiple specific elements
 * 
 * @param page_va   the virtual address of a page
 * @param type_size specific type
 * @param is_cycle  whether the list is a cycle list
 */
void
fmtlist_init(fmtlist_t *page_va, uint32_t type_size, bool is_cycle) {
    if (page_va == null)    panic("fmtlist_init(): parameter invalid");

    uint8_t *p = (uint8_t *)(PGDOWN((uint32_t)page_va, PGSIZE));
    uint32_t head_sz = sizeof(fmtlist_t);
    uint32_t elem_sz = sizeof(node_t) + type_size;
    fmtlist_t *fmtlist = (fmtlist_t *)p;
    fmtlist->type_size_ = type_size;
    fmtlist->total_elems_ = (PGSIZE - head_sz) / elem_sz;

    /*
     * transform to the following formatting
     * ┌───────────┬────────┬──────────┬─────────┬────────┬──────────┬────┐
     * │ fmtlist_t │ node_t │   xx_t   │         │ node_t │   xx_t   │    │
     * │           │                   │   ...   │                   │ .. │
     * │  (head)   │      (elem 1)     │         │      (elem x)     │    │
     * └───────────┴────────┴──────────┴─────────┴────────┴──────────┴────┘
     */

    list_init(&fmtlist->list_, is_cycle);
    p += head_sz;
    for (uint32_t i = 0; i < fmtlist->total_elems_; ++i) {
        node_t *n = (node_t *)p;
        uint8_t *type_elem = (uint8_t *)(p + sizeof(node_t));
        p += elem_sz;
        node_set(n, type_elem, (node_t *)p);
        list_insert(&fmtlist->list_, n, LSIDX_AFTAIL(&fmtlist->list_));
    }
}

/**
 * @brief get a element from the formatting list
 * 
 * @param fmtlist formatting list
 * @return the specific element
 */
void *
fmtlist_alloc(fmtlist_t *fmtlist) {
    if (fmtlist == null)    panic("fmtlist_alloc(): parameter invalid");
    return list_remove(&fmtlist->list_, 1)->data_;
}

/**
 * @brief reclaim the list elements of specific type
 * 
 * @param fmtlist   the formatting list
 * @param elem      the free element
 * @param elem_size the length of the free element
 */
void
fmtlist_release(fmtlist_t *fmtlist, void *elem, uint32_t elem_size) {
    if (fmtlist == null)    panic("fmtlist_release(): parameter invalid");
    if (elem_size != fmtlist->type_size_)
        panic("fmtlist_release(): element type not match");

    if (elem) {
        uint8_t *p = (uint8_t *)elem;
        p -= sizeof(node_t);
        bzero(p, sizeof(node_t) + fmtlist->type_size_);
        ((node_t *)p)->data_ = elem;
        list_insert(&fmtlist->list_, (node_t *)p, 1);
    }
}
