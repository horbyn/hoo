/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm.h"

/**
 * @brief initialize virtual memory system
 */
void
init_virmm_system() {
    init_kern_virmm_bitmap();
    init_metadata();
}

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
 * @brief request some free virtual pages
 * 
 * @param vspace the virtual space manager
 * @param amount request page amounts
 * @param begin  the beginning address to be searched
 * @return a virtual address followed serveral pages available;
 * null if failure occured
 */
void *
vir_alloc_pages(vspace_t *vspace, uint32_t amount, uint32_t begin) {
    if (vspace == null)    panic("vir_alloc_pages(): null pointer");
    if (amount == 0)    panic("vir_alloc_pages(): cannot request 0 page");

    // traversal the virtual space
    const uint32_t MAX_END =
        begin < KERN_HIGH_MAPPING ? KERN_HIGH_MAPPING : (PG_MASK - MB4 + PGSIZE);
    uint32_t last_end = begin, ret = 0;
    vspace_t *worker = vspace;

    while (worker != null) {

        if (worker->next_ != null
            && (((worker->next_->begin_ - last_end) / PGSIZE) < amount)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->end_;
            worker = worker->next_;
        } else {
            // there is enough space; or there is no next interval
            if (worker->next_ == null
                && last_end + amount * PGSIZE >= MAX_END)
                panic("vir_alloc_pages(): no enough space");

            vspace_t *temp = worker;
            if (list_isempty(&worker->list_)) {
                temp = vspace_alloc();
                vspace_set(temp, null, last_end, last_end, null);
            }
            node_t   *node_free = node_alloc();
            vaddr_t  *vaddr_free = vaddr_alloc();

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

/**
 * @brief release some virtual addresses
 * 
 * @param vspace the virtual space object
 * @param va     the beginning virtual address
 * @param rel    whether release the physical page corresponding the virtual address
 */
void
vir_release_pages(vspace_t *vspace, void *va, bool rel) {
    if (vspace == null)    panic("vir_release_pages(): null pointer");

    // search the `vspace` list according to virtual address
    vspace_t *worker_vs = vspace, *prev_vs = null;
    while (worker_vs != null) {
        if (worker_vs->begin_ <= (uint32_t)va && (uint32_t)va < worker_vs->end_)
            break;
        prev_vs = worker_vs;
        worker_vs = worker_vs->next_;
    }
    if (worker_vs == null)    panic("vir_release_pages(): invalid virtual address");

    enum location_e {
        LCT_BEGIN = 0,
        LCT_MIDDLE,
        LCT_END
    } lct;

    // search the `vaddr` object according to virtual address
    node_t *worker_node = null;
    int i = 1;
    vspace_t *new_vs = null;
    do {
        worker_node = list_find(&worker_vs->list_, i);
        if (worker_node != null) {
            if ((uint32_t)va == ((vaddr_t*)(worker_node->data_))->va_) {

                lct = (i == 1) ? LCT_BEGIN : ((i == worker_vs->list_.size_)
                    ? LCT_END : LCT_MIDDLE);
                list_remove(&worker_vs->list_, i);

                if (lct == LCT_MIDDLE) {
                    // it is the middle node
                    new_vs = vspace_alloc();
                    for (uint32_t j = i, k = 1; j <= worker_vs->list_.size_;) {
                        node_t *n = list_remove(&worker_vs->list_, j);
                        list_insert(&new_vs->list_, n, k++);
                        prev_vs->next_ = new_vs;
                    } // end for(j)
                    vspace_append(new_vs, worker_vs);
                }
                break;
            }
        }
        ++i;
    } while (worker_node != null);
    // previously, the virtual address is belong to the current list,
    //   so it must exist in current list.
    if (worker_node == null)    panic("vir_release_pages(): bug");

    // release pages
    uint32_t pages_amount = ((vaddr_t *)(worker_node->data_))->length_;
    if (rel) {
        for (uint32_t i = 0; i < pages_amount; ++i) {
            uint32_t va = ((vaddr_t *)(worker_node->data_))->va_;
            phy_release_vpage((void *)(va + i * PGSIZE));
        }
    }

    // reclaim `vspace` list
    if (list_isempty(&worker_vs->list_)) {
        vspace_append(prev_vs, worker_vs->next_);
        vspace_release(worker_vs);
    } else {
        if (lct == LCT_BEGIN)    worker_vs->end_ -= (pages_amount * PGSIZE);
        else if (lct == LCT_END)    worker_vs->begin_ += (pages_amount * PGSIZE);
        else {
            if (new_vs == null)    panic("vir_release_pages(): bug");
            worker_vs->begin_ = ((vaddr_t *)(worker_node->data_))->va_
                + ((vaddr_t *)(worker_node->data_))->length_ * PGSIZE;
            new_vs->end_ = ((vaddr_t *)(worker_node->data_))->va_;
            node_t *n = list_find(&new_vs->list_, new_vs->list_.size_);
            new_vs->begin_ = ((vaddr_t *)(n->data_))->va_;
        }
    }

    // reclaim the metadata
    node_release(worker_node);
    vaddr_release(worker_node->data_);
}
