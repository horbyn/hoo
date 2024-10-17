/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vmngr.h"

/**
 * @brief Set the vaddr object
 * 
 * @param vaddr the virtual address object
 * @param addr the virtual address
 * @param length the pages followed by this virtual address
 */
void
vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length) {
    if (vaddr == null)    panic("vaddr_set(): null pointer");
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

    if (vs == null)    panic("vspace_set(): null pointer");
    if (ls != null && &vs->list_ != ls)
        memmove(&vs->list_, ls, sizeof(list_t));
    else    bzero(&vs->list_, sizeof(list_t));
    vs->begin_ = begin;
    vs->end_ = end;
    if (next == null)    vs->next_ = null;
    else    vs->next_ = next;
}

/**
 * @brief `vsmngr` object initialization
 * 
 * @param mngr `vsmngr` object
 * @param va_vs metadata vspace virtual address
 * @param va_node metadata node virtual address
 * @param va_vaddr metadata vaddr virtual address
 */
void
vsmngr_set(vsmngr_t *mngr, void *va_vs, void *va_node, void *va_vaddr) {
    if (mngr == null)    panic("vsmngr_set(): null pointer");
    if (va_vs == null)    mngr->vspace_ = null;
    else    mngr->vspace_ = va_vs;
    if (va_node == null)    mngr->node_ = null;
    else    mngr->node_ = va_node;
    if (va_vaddr == null)    mngr->vaddr_ = null;
    else    mngr->vaddr_ = va_vaddr;
}
