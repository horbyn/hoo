/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "vaddr.h"

/**
 * @brief virtual address space initialization
 * 
 * @param space virtual address space
 */
void
vaddr_init(vaddr_list_t *space) {
    bzero(space, sizeof(vaddr_list_t));
}

/**
 * @brief get a free space (only 1 page per allocation)
 * 
 * @param space virtual address space
 * @param size the size of space (in pages)
 * @return virtual address base
 */
void *
vaddr_alloc(vaddr_list_t *space, uint32_t size) {
    if (space == null)    panic("vaddr_alloc(): invalid parameter");

    const uint32_t ADDR_BASE = 0x00000000;
    uint32_t last_end = ADDR_BASE, ret = 0;
    vaddr_list_t *worker = space;

    while (worker) {

        if (worker->next_ && (((worker->next_->inte_.begin_ - last_end) / PGSIZE) < size)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->inte_.end_;
            worker = worker->next_;
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
