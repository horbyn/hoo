/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vaddr.h"

/**
 * @brief Set the vaddr object
 * 
 * @param vaddr the virtual address object
 * @param addr the virtual address
 * @param length the pages followed by this virtual address
 */
void
vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length) {
    if (vaddr == null)    panic("vaddr_set(): invalid vaddr object");
    vaddr->va_ = addr;
    vaddr->length_ = length;
}

/**
 * @brief Set the vaddr list object
 * 
 * @param vs    the virtual address space object
 * @param ls    the list object
 * @param begin the beginning endpoint of the interval
 * @param end   the ending endpoint of the interval
 * @param next  the next virtual space
 */
void
vspace_set(vspace_t *vs, list_t *ls, uint32_t begin, uint32_t end,
               vspace_t *next) {

    if (vs == null || ls == null)    panic("vspace_set(): invalid virtual space");
    if (&vs->list_ != ls)
        memmove(&vs->list_, ls, sizeof(list_t));
    vs->begin_ = begin;
    vs->end_ = end;
    vs->next_ = next;
}

/**
 * @brief append `next` to `cur`
 * 
 * @param cur current vspace object
 * @param next next vspace object
 */
void
vspace_append(vspace_t *cur, vspace_t *next) {
    if (cur == null || next == null)
        panic("vspace_append(): invalid parameter");

    next->next_ = cur->next_;
    cur->next_ = next;
}
