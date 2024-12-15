/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vspace.h"
#include "kern/panic.h"
#include "user/lib.h"
#include "user/lib.h"
#include "kern/module/io.h"

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
    else {
        vs->list_.null_.data_ = vs->list_.null_.next_ = null;
        vs->list_.size_ = 0;
    }
    vs->begin_ = begin;
    vs->end_ = end;
    if (next == null)    vs->next_ = null;
    else    vs->next_ = next;
}

/**
 * @brief print the vsmngr object
 * 
 * @param vs virtual space object
 */
void
debug_print_vsmngr(vspace_t *vs) {
    if (vs == null)    return;
    vspace_t *vspace = vs->next_;
    while (vspace != null) {
        kprintf("\t.begin=0x%x, .end=0x%x, .next=0x%x\n\t.list: \n",
            vspace->begin_, vspace->end_, vspace->next_);
        list_t *list = &(vspace->list_);
        for (int i = 1; i <= list->size_; ++i) {
            node_t *n = list_find(list, i);
            vaddr_t *va = n->data_;
            kprintf("[0x%x, 0x%x]", va->va_, va->length_);
            if (n->next_)    kprintf(", ");
        }
        kprintf("\n");
        vspace = vspace->next_;
    }
}
