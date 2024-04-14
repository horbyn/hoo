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

/**
 * @brief `vsmngr` object initialization
 * 
 * @param mngr `vsmngr` object
 * @param va_vs metadata vspace virtual address
 * @param va_node metadata node virtual address
 * @param va_vaddr metadata vaddr virtual address
 */
void
vsmngr_init(vsmngr_t *mngr, void *va_vs, void *va_node, void *va_vaddr) {
    if (mngr == null)    panic("vsmngr_init(): invalid parameter");
    mngr->vspace_ = va_vs;
    mngr->node_ = va_node;
    mngr->vaddr_ = va_vaddr;
}

/**
 * @brief metadata vspace allocation
 * 
 * @param mngr `vsmngr` object
 */
vspace_t *
vsmngr_alloc_vspace(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vspace(): invalid parameter");
    if (list_isempty(&mngr->vspace_->list_))
        fmtlist_init(mngr->vspace_, sizeof(vspace_t), true);
    return fmtlist_alloc(mngr->vspace_);
}

/**
 * @brief metadata node allocation
 * 
 * @param mngr `vsmngr` object
 */
node_t *
vsmngr_alloc_node(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_node(): invalid parameter");
    if (list_isempty(&mngr->node_->list_))
        fmtlist_init(mngr->node_, sizeof(node_t), true);
    return fmtlist_alloc(mngr->node_);
}

/**
 * @brief metadata vaddr allocation
 * 
 * @param mngr `vsmngr` object
 */
vaddr_t *
vsmngr_alloc_vaddr(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vaddr(): invalid parameter");
    if (list_isempty(&mngr->vaddr_->list_))
        fmtlist_init(mngr->vaddr_, sizeof(vaddr_t), true);
    return fmtlist_alloc(mngr->vaddr_);
}

/**
 * @brief metadata vspace releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata vspace object
 */
void
vsmngr_release_vspace(vsmngr_t *mngr, vspace_t *vs) {
    fmtlist_release(mngr->vspace_, vs, sizeof(vspace_t));
}

/**
 * @brief metadata node releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata node object
 */
void
vsmngr_release_node(vsmngr_t *mngr, node_t *node) {
    fmtlist_release(mngr->node_, node, sizeof(node_t));
}

/**
 * @brief metadata vaddr releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata vaddr object
 */
void
vsmngr_release_vaddr(vsmngr_t *mngr, vaddr_t *vaddr) {
    fmtlist_release(mngr->vaddr_, vaddr, sizeof(vaddr_t));
}
