/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm.h"

/**
 * @brief append `next` to `cur`
 * 
 * @param cur current vspace object
 * @param next next vspace object
 */
static void
vspace_append(vspace_t *cur, vspace_t *next) {
    if (cur == null)    panic("vspace_append(): null pointer");
    if (next == null)    cur->next_ = null;
    else    cur->next_ = next;
}


/**
 * @brief metadata vspace allocation
 * 
 * @param mngr `vsmngr` object
 */
static vspace_t *
vsmngr_alloc_vspace(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vspace(): null pointer");
    if (list_isempty(&mngr->vspace_->list_))
        fmtlist_init(mngr->vspace_, sizeof(vspace_t), true);
    vspace_t *p = fmtlist_alloc(mngr->vspace_);
    if (p == null)    panic("vsmngr_alloc_vspace(): no enough memory");
    return p;
}

/**
 * @brief metadata node allocation
 * 
 * @param mngr `vsmngr` object
 */
static node_t *
vsmngr_alloc_node(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_node(): null pointer");
    if (list_isempty(&mngr->node_->list_))
        fmtlist_init(mngr->node_, sizeof(node_t), true);
    node_t *p = fmtlist_alloc(mngr->node_);
    if (p == null)    panic("vsmngr_alloc_node(): no enough memory");
    return p;
}

/**
 * @brief metadata vaddr allocation
 * 
 * @param mngr `vsmngr` object
 */
static vaddr_t *
vsmngr_alloc_vaddr(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vaddr(): null pointer");
    if (list_isempty(&mngr->vaddr_->list_))
        fmtlist_init(mngr->vaddr_, sizeof(vaddr_t), true);
    vaddr_t *p = fmtlist_alloc(mngr->vaddr_);
    if (p == null)    panic("vsmngr_alloc_vaddr(): no enough memory");
    return p;
}

/**
 * @brief request some free virtual pages
 * 
 * @param vmngr  the virtual space manager
 * @param amount request page amounts
 * @param begin  the beginning address to be searched
 * @param end    the ending address to be searched
 * @return a virtual address followed serveral pages available;
 * null if failure occured
 */
void *
vir_alloc_pages(vsmngr_t *vmngr, uint32_t amount, uint32_t begin, uint32_t end) {
    if (vmngr == null)    panic("vir_alloc_pages(): null pointer");
    if (amount == 0)    panic("vir_alloc_pages(): cannot request 0 page");

    // traversal the virtual space
    uint32_t last_end = begin, ret = 0;
    vspace_t *worker = &vmngr->head_;

    while (worker) {

        if (worker->next_ && (((worker->next_->begin_ - last_end) / PGSIZE) < amount)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->end_;
            worker = worker->next_;
        } else {
            // there is enough space; or there is no next interval
            if (!worker->next_ && last_end + amount * PGSIZE >= end)
                panic("vir_alloc_pages(): no enough space");

            vspace_t *temp = worker;
            if (list_isempty(&worker->list_)) {
                temp = vsmngr_alloc_vspace(vmngr);
                vspace_set(temp, null, last_end, last_end, null);
            }
            node_t   *node_free = vsmngr_alloc_node(vmngr);
            vaddr_t  *vaddr_free = vsmngr_alloc_vaddr(vmngr);

            vaddr_set(vaddr_free, last_end, amount);
            node_set(node_free, vaddr_free, null);
            list_insert(&temp->list_, node_free, 1);

            if (list_isempty(&worker->list_))    vspace_append(worker, temp);

            ret = last_end;
            temp->end_ += amount * PGSIZE;
            break;
        }

    } // end while()

    return (void *)ret;
}
