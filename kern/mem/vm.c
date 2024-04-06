/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm.h"

static spinlock_t __spinlock_virmm;
/**
 * @brief metadata provider of the virtual memory module;
 * access this structure by macro or functions because
 * I expect the members are invisiable outside
 */
static struct vm_slot {
    // to specify which thread
    tid_t     tid_;
    // the metadata
    fmtlist_t *vs_free_, *node_free_, *vaddr_free_;
} __vm_slot[MAX_TASKS_AMOUNT];

/**
 * @brief initialize virtual memory system
 */
void
init_virmm_system() {
    spinlock_init(&__spinlock_virmm);
    bzero(__vm_slot, sizeof(__vm_slot));
}

/**
 * @brief metadata initialization
 * 
 * @param va virtual address of a page
 * @param mdata_size metadata size
 */
static void
metadata_init(void *va, uint32_t mdata_size) {
    void *pa = phy_alloc_page();
    void *v = (void *)PGDOWN((uint32_t)va, PGSIZE);
    pcb_t *cur_pcb = get_current_pcb();
    if (cur_pcb != __pcb_idle)
        panic("metadata_init(): not allow other kernel threads to set mapping");
    set_mapping(cur_pcb->pdir_va_, (uint32_t)v, (uint32_t)pa);
    fmtlist_init(v, mdata_size, true);
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

    pcb_t *cur_pcb = get_current_pcb();
    struct vm_slot *slot = __vm_slot + tid;

    // need more page to hold the metadata
    bool fvs = false, fnode = false, fvaddr = false;
    if (slot->vs_free_ == null) {
        ++amount;
        fvs = true;
    }
    if (slot->node_free_ == null) {
        ++amount;
        fnode = true;
    }
    if (slot->vaddr_free_ == null) {
        ++amount;
        fvaddr = true;
    }

    // traversal the virtual space
    const uint32_t ADDR_BASE = cur_pcb == __pcb_idle ?
        KERN_HIGH_MAPPING + MM_BASE : 0x00000000;
    const uint32_t ADDR_END = cur_pcb == __pcb_idle ?
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
            if (!worker->next_ && last_end + amount >= ADDR_END)
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
            if (fvs) {
                metadata_init(va_mdata_vs, sizeof(vspace_t));
                slot->vs_free_ = (fmtlist_t *)va_mdata_vs;
            }
            if (fnode) {
                metadata_init(va_mdata_node, sizeof(node_t));
                slot->node_free_ = (fmtlist_t *)va_mdata_node;
            }
            if (fvaddr) {
                metadata_init(va_mdata_vaddr, sizeof(vaddr_t));
                slot->vaddr_free_ = (fmtlist_t *)va_mdata_vaddr;
            }

            vspace_t *temp = list_isempty(&worker->list_) ?
                (vspace_t *)fmtlist_alloc(slot->vs_free_) : worker;
            node_t   *node_free = null;
            vaddr_t  *vaddr_free = null;

            // map metadata
            if (fvs) {
                node_free = (node_t *)fmtlist_alloc(slot->node_free_);
                vaddr_free = (vaddr_t *)fmtlist_alloc(slot->vaddr_free_);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_vs, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }
            if (fnode) {
                node_free = (node_t *)fmtlist_alloc(slot->node_free_);
                vaddr_free = (vaddr_t *)fmtlist_alloc(slot->vaddr_free_);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_node, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }
            if (fvaddr) {
                node_free = (node_t *)fmtlist_alloc(slot->node_free_);
                vaddr_free = (vaddr_t *)fmtlist_alloc(slot->vaddr_free_);
                vaddr_set(vaddr_free, (uint32_t)va_mdata_vaddr, 1);
                node_set(node_free, vaddr_free, null);
                list_insert(&temp->list_, node_free, 1);

                last_end += PGSIZE;
                --amount;
            }

            node_free = (node_t *)fmtlist_alloc(slot->node_free_);
            vaddr_free = (vaddr_t *)fmtlist_alloc(slot->vaddr_free_);
            vaddr_set(vaddr_free, last_end, amount);
            node_set(node_free, vaddr_free, null);
            list_insert(&temp->list_, node_free, 1);

            if (list_isempty(&worker->list_)) {
                // list append: worker->temp
                vspace_append(worker, temp);
            }

            ret = last_end;
            temp->end_ = last_end + amount * PGSIZE;
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
    struct vm_slot *slot = __vm_slot + tid;
    fmtlist_release(slot->vaddr_free_, worker_node->data_, sizeof(vaddr_t));
    fmtlist_release(slot->node_free_, worker_node, sizeof(node_t));
    if (slot->vaddr_free_->total_elems_ == slot->vaddr_free_->list_.size_) {
        pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)slot->vaddr_free_);
        phy_release_page((void *)*pa);
        slot->vaddr_free_ = null;
    }
    if (slot->node_free_->total_elems_ == slot->node_free_->list_.size_) {
        pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)slot->node_free_);
        phy_release_page((void *)*pa);
        slot->node_free_ = null;
    }

    // reclaim `vspace` list
    if (list_isempty(&worker_vs->list_)) {
        vspace_append(prev_vs, worker_vs->next_);
        fmtlist_release(slot->vs_free_, worker_vs, sizeof(vspace_t));
        if (slot->vs_free_->total_elems_ == slot->vs_free_->list_.size_) {
            pgelem_t *pa = get_mapping(cur_pcb->pdir_va_, (uint32_t)slot->vs_free_);
            phy_release_page((void *)*pa);
            slot->vs_free_ = null;
        }
    }
}
