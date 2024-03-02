/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "vaddr.h"

/**
 * @brief Set the vaddr list object
 * 
 * @param dlist the virtual address double-list
 * @param llist the virtual address linked-list
 * @param begin the beginning endpoint of the interval
 * @param end the ending endpoint of the interval
 * @param next the next double-list
 */
void set_vaddrlist(vaddr_list_t *dlist, list_t *llist, uint32_t begin, uint32_t end, vaddr_list_t *next) {
    if (dlist == null)    panic("set_vaddrlist(): invalid double list");
    dlist->list_ = llist;
    dlist->inte_.begin_ = begin;
    dlist->inte_.end_ = end;
    dlist->next_ = next;
}

/**
 * @brief Set the vaddr object
 * 
 * @param vaddr the virtual address object
 * @param addr the virtual address
 * @param size the pages followed by this virtual address
 */
void
set_vaddr(vaddr_t *vaddr, uint32_t addr, uint32_t size) {
    if (vaddr == null)    panic("set_vaddr(): invalid vaddr object");
    vaddr->addr_ = addr;
    vaddr->size_ = size;
}

/**
 * @brief get a free space (only 1 page per allocation)
 * 
 * @param space virtual address space
 * @param size the size of space (in pages)
 * @return the beginning virtual address which the *size* pages followed by are free
 */
void *
vaddr_get(vaddr_list_t *space, uint32_t size) {
    if (space == null)    panic("vaddr_get(): invalid parameter");

    const uint32_t ADDR_BASE = 0x00000000;
    uint32_t last_end = ADDR_BASE, ret = 0;
    vaddr_list_t *worker = space;

    while (worker) {

        if (worker->next_ && (((worker->next_->inte_.begin_ - last_end) / PGSIZE) < size)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->inte_.end_;
            worker = worker->next_;
        } else if (last_end + size >= MAX_SPACE) {
            panic("vaddr_get(): no enough space");
        } else {
            // there is enough space; or there is no next interval

            ret = last_end;

            // //
            if (worker->list_) {
                ;
            } else {
                ;
            }
            // //

            break;
        }

    } // end while()

    return (void *)ret;
}
