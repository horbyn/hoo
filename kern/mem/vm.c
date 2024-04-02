/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm.h"

static vmslot_t __vm_slot[MAX_TASKS_AMOUNT];
static spinlock_t __spinlock_virmm;

/**
 * @brief vmslot object setup thread id
 * 
 * @param slot vmslot object
 * @param tid  thread id
 */
void
vmslot_set_tid(vmslot_t *slot, tid_t tid) {
    if (slot == null)    panic("vmslot_set_tid(): invalid parameter");
    slot->tid_ = tid;
}

/**
 * @brief vmslot object setup virtual address space
 * 
 * @param slot vmslot object
 * @param va virtual address of the virtual address space
 */
void
vmslot_set_vspace(vmslot_t *slot, void *va) {
    if (slot == null)    panic("vmslot_set_vspace(): invalid parameter");
    GET_FREE_LIST(slot->vs_free_, va, vspace_t);
    slot->vs_cnt_ = GET_FREE_LIST_LEN(vspace_t);
}

/**
 * @brief vmslot object setup node
 * 
 * @param slot vmslot object
 * @param va virtual address of the node
 */
void
vmslot_set_node(vmslot_t *slot, void *va) {
    if (slot == null)    panic("vmslot_set_node(): invalid parameter");
    GET_FREE_LIST(slot->node_free_, va, node_t);
    slot->node_cnt_ = GET_FREE_LIST_LEN(node_t);
}

/**
 * @brief vmslot object setup vaddr object
 * 
 * @param slot vmslot object
 * @param va virtual address of the vaddr object
 */
void
vmslot_set_vaddr(vmslot_t *slot, void *va) {
    if (slot == null)    panic("vmslot_set_vaddr(): invalid parameter");
    GET_FREE_LIST(slot->vaddr_free_, va, vaddr_t);
    slot->vaddr_cnt_ = GET_FREE_LIST_LEN(vaddr_t);
}

/**
 * @brief get the slot object according to thread id for storing metadata
 * 
 * @param tid thread id
 * @return slot object (need null verification)
 */
vmslot_t *
vmslot_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)    panic("vmslot_get(): invalid thread id");
    return &__vm_slot[tid];
}

/**
 * @brief get free vspace object
 * 
 * @param slot vmslot object
 * @return vspace object
 */
vspace_t *
vmslot_get_vspace(vmslot_t *slot) {
    if (slot == null)    panic("vmslot_get_vspace(): invalid parameter");
    return list_remove(slot->vs_free_, 1)->data_;
}

/**
 * @brief get free node object
 * 
 * @param slot vmslot object
 * @return node object
 */
node_t *
vmslot_get_node(vmslot_t *slot) {
    if (slot == null)    panic("vmslot_get_node(): invalid parameter");
    return list_remove(slot->node_free_, 1)->data_;
}

/**
 * @brief get free vaddr object
 * 
 * @param slot vmslot object
 * @return vaddr object
 */
vaddr_t *
vmslot_get_vaddr(vmslot_t *slot) {
    if (slot == null)    panic("vmslot_get_vaddr(): invalid parameter");
    return list_remove(slot->vaddr_free_, 1)->data_;
}

/**
 * @brief reclaim the vspace object into `vmslot`
 * 
 * @param slot `vmslot` object
 * @param vs `vspace` object
 */
void
vmslot_reclaim_vspace(vmslot_t *slot, vspace_t *vs) {
    RECLAIM_METADATA(slot->vs_free_, vs, vspace_t);
}

/**
 * @brief reclaim the node object into `vmslot`
 * 
 * @param slot `vmslot` object
 * @param node `node` object
 */
void
vmslot_reclaim_node(vmslot_t *slot, node_t *node) {
    RECLAIM_METADATA(slot->node_free_, node, node_t);
}

/**
 * @brief reclaim the vaddr object into `vmslot`
 * 
 * @param slot `vmslot` object
 * @param vaddr `vaddr` object
 */
void
vmslot_reclaim_vaddr(vmslot_t *slot, vaddr_t *vaddr) {
    RECLAIM_METADATA(slot->vaddr_free_, vaddr, vaddr_t);
}

/**
 * @brief initialize virtual memory system
 */
void
init_virmm_system() {
    spinlock_init(&__spinlock_virmm);
    bzero(__vm_slot, sizeof(__vm_slot));
}

/**
 * @brief request some free virtual pages
 * 
 * @param tid thread id
 * @param vs  virtual address space object
 * @param amount request page amounts
 * @return a virtual address followed serveral pages available;
 * null if failure occured
 */
void *
vir_alloc_pages(tid_t tid, vspace_t *vs, uint32_t amount) {
    if (vs == null)    panic("vir_alloc_pages(): invalid virtual space");
    if (amount == 0)    panic("vir_alloc_pages(): cannot request 0 page");

    // search the slot
    vmslot_t *slot = vmslot_get(tid);
    if (slot == null)
        panic("vir_alloc_pages(): no free slot for current task");

    // need more page to hold the metadata
    bool fvs = false, fnode = false, fvaddr = false;
    if (ISEMPTY_LIST_VSPACE(slot)) {
        ++amount;
        fvs = true;
    }
    if (ISEMPTY_LIST_NODE(slot)) {
        ++amount;
        fnode = true;
    }
    if (ISEMPTY_LIST_VADDR(slot)) {
        ++amount;
        fvaddr = true;
    }

    // traversal the virtual space
    const uint32_t ADDR_BASE = 0x00000000;
    uint32_t addr_end = get_current_pcb() == __pcb_idle ?
        MAX_VSPACE_IDLE : KERN_HIGH_MAPPING;
    uint32_t last_end = ADDR_BASE, ret = 0;
    vspace_t *worker = vs;

    while (worker) {

        if (worker->next_ && (((worker->next_->begin_ - last_end) / PGSIZE) < amount)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->end_;
            worker = worker->next_;
        } else {
            // there is enough space; or there is no next interval
            if (!worker->next_ && last_end + amount >= addr_end)
                panic("vir_alloc_pages(): no enough space");

            // virtual address allocation
            void *va_mdata_vs = fvs ? (void *)last_end : null;
            void *va_mdata_node   = fnode ?
                (va_mdata_vs ? va_mdata_vs + PGSIZE : (void *)last_end)
                : null;
            void *va_mdata_vaddr = fvaddr ?
                (va_mdata_node ?
                    va_mdata_node + PGSIZE
                    : (va_mdata_vs ? va_mdata_vs + PGSIZE : (void *)last_end))
                : null;

            // check metadata
            if (fvs)
                vmslot_set_vspace(slot, va_mdata_vs);
            if (fnode)
                vmslot_set_node(slot, va_mdata_node);
            if (fvaddr)
                vmslot_set_vaddr(slot, va_mdata_vaddr);

            vspace_t *temp = list_isempty(&worker->list_) ?
                vmslot_get_vspace(slot) : worker;
            node_t   *node_free = null;
            vaddr_t  *vaddr_free = null;

            // map metadata
            if (fvs) {
                node_free = vmslot_get_node(slot);
                vaddr_free = vmslot_get_vaddr(slot);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_vs, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }
            if (fnode) {
                node_free = vmslot_get_node(slot);
                vaddr_free = vmslot_get_vaddr(slot);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_node, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }
            if (fvaddr) {
                node_free = vmslot_get_node(slot);
                vaddr_free = vmslot_get_vaddr(slot);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_vaddr, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }

            node_free = vmslot_get_node(slot);
            vaddr_free = vmslot_get_vaddr(slot);
            vaddr_set(vaddr_free, last_end, amount);
            node_set(node_free, vaddr_free, null);
            list_insert(&temp->list_, node_free, 1);

            if (list_isempty(&worker->list_)) {
                // list append: worker->temp
                vspace_append(worker, temp);
            }

            ret = last_end;
            worker->end_ += amount * PGSIZE;
            break;
        }

    } // end while()

    return (void *)ret;
}

/**
 * @brief release some virtual addresses
 * 
 * @param tid the thread id of the thread need to release virtual memory
 * @param vs `vspace` object
 * @param va the beginning virtual address
 */
void
vir_release_pages(tid_t tid, vspace_t *vs, void *va) {
    if (vs == null)
        panic("vir_release_pages(): paremeter invalid");

    // search the `vspace` list according to virtual address
    vspace_t *worker_vs = vs, *prev_vs = null;
    while (worker_vs) {
        if (worker_vs->begin_ <= (uint32_t)va && (uint32_t)va < worker_vs->end_)
            break;
        prev_vs = worker_vs;
        worker_vs = worker_vs->next_;
    }
    if (worker_vs == null)    panic("vir_release_pages(): invalid virtual address");

    // search the `vaddr` object according to virtual address
    node_t *worker_node = null;
    idx_t i = 1;
    do {
        worker_node = list_find(&worker_vs->list_, i);
        if (worker_node) {
            if ((uint32_t)va == ((vaddr_t*)(worker_node->data_))->va_) {
                list_remove(&worker_vs->list_, i);
                break;
            }
        }
        ++i;
    } while (worker_node);
    // previously, the virtual address is belong to the current list,
    //   so it must exist in current list.
    if (worker_node == null)    panic("vir_release_pages(): bug");

    // release pages
    pcb_t *cur_pcb = get_current_pcb();
    for (uint32_t i = 0; i < ((vaddr_t *)(worker_node->data_))->length_; ++i) {
        uint32_t va = ((vaddr_t *)(worker_node->data_))->va_;
        pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, va + i * PGSIZE);
        phy_release_page((void *)*pa);
    }

    // reclaim the metadata
    vmslot_t *vmslot = vmslot_get(tid);
    vmslot_reclaim_vaddr(vmslot, (vaddr_t*)(worker_node->data_));
    vmslot_reclaim_node(vmslot, worker_node);
    if (vmslot->vaddr_cnt_ == vmslot->vaddr_free_->size_) {
        pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)vmslot->vaddr_free_);
        phy_release_page((void *)*pa);
        vmslot->vaddr_free_ = null;
    }
    if (vmslot->node_cnt_ == vmslot->node_free_->size_) {
        pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)vmslot->node_free_);
        phy_release_page((void *)*pa);
        vmslot->node_free_ = null;
    }

    // reclaim `vspace` list
    if (list_isempty(&worker_vs->list_)) {
        vspace_append(prev_vs, worker_vs->next_);
        vmslot_reclaim_vspace(vmslot, worker_vs);
        if (vmslot->vs_cnt_ == vmslot->vs_free_->size_) {
            pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)vmslot->vs_free_);
            phy_release_page((void *)*pa);
            vmslot->vs_free_ = null;
        }
    }
}
