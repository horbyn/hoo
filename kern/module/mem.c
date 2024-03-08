/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "mem.h"

static vaddr_list_t __kern_double_list;
static list_t __kern_linked_list;
static node_t __kern_fist_node;
static vaddr_t __kern_first_vaddr;

void
init_virtual_memory() {
    pcb_t *kern_pcb = get_pcb();
    bzero(&kern_pcb->vspace_, sizeof(vaddr_list_t));

    // filling the lowest 1MB space
    set_vaddr(&__kern_first_vaddr, 0, MM_BASE / PGSIZE);
    __kern_fist_node.data_ = &__kern_first_vaddr;
    __kern_fist_node.next_ = null;
    list_insert(&__kern_linked_list, &__kern_fist_node, __kern_linked_list.size_ + 1);
    set_vaddrlist(&__kern_double_list, &__kern_linked_list, 0, MM_BASE, null);

    kern_pcb->vspace_.next_ = &__kern_double_list;
}
