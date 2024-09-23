/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm.h"

static spinlock_t __spinlock_virmm;

#ifdef DEBUG
/**
 * @brief debug mode: print virtual space of current pcb
 */
void
debug_print_vspace_pcb(pcb_t *pcb) {
    vspace_t *vspace = pcb->vmngr_.head_.next_;
    while (vspace) {
        kprintf("[DEBUG] .begin=0x%x, .end=0x%x, .next=0x%x\n[DEBUG] .list: \n",
            vspace->begin_, vspace->end_, vspace->next_);
        list_t *list = &(vspace->list_);
        for (idx_t i = 1; i <= list->size_; ++i) {
            node_t *n = list_find(list, i);
            vaddr_t *va = n->data_;
            kprintf("[0x%x, 0x%x]", va->va_, va->length_);
            if (n->next_)    kprintf(", ");
        }
        kprintf("\n");
        vspace = vspace->next_;
    }
}
#endif

/**
 * @brief initialize virtual memory system
 */
void
init_virmm_system() {
    spinlock_init(&__spinlock_virmm);
}

/**
 * @brief append `next` to `cur`
 * 
 * @param cur current vspace object
 * @param next next vspace object
 */
static void
vspace_append(vspace_t *cur, vspace_t *next) {
    if (cur == null)
        panic("vspace_append(): invalid parameter");
    cur->next_ = next;
}


/**
 * @brief metadata vspace allocation
 * 
 * @param mngr `vsmngr` object
 */
static vspace_t *
vsmngr_alloc_vspace(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vspace(): invalid parameter");
    if (list_isempty(&mngr->vspace_->list_))
        fmtlist_init(mngr->vspace_, sizeof(vspace_t), true);
    vspace_t *p = fmtlist_alloc(mngr->vspace_);
    if (p == null)    panic("vsmngr_alloc_vspace(): no enough memory");
    return p;
}

/**
 * @brief metadata node allocation
 * 
 * @param mngr `vsmngr` object
 */
static node_t *
vsmngr_alloc_node(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_node(): invalid parameter");
    if (list_isempty(&mngr->node_->list_))
        fmtlist_init(mngr->node_, sizeof(node_t), true);
    node_t *p = fmtlist_alloc(mngr->node_);
    if (p == null)    panic("vsmngr_alloc_node(): no enough memory");
    return p;
}

/**
 * @brief metadata vaddr allocation
 * 
 * @param mngr `vsmngr` object
 */
static vaddr_t *
vsmngr_alloc_vaddr(vsmngr_t *mngr) {
    if (mngr == null)    panic("vsmngr_alloc_vaddr(): invalid parameter");
    if (list_isempty(&mngr->vaddr_->list_))
        fmtlist_init(mngr->vaddr_, sizeof(vaddr_t), true);
    vaddr_t *p = fmtlist_alloc(mngr->vaddr_);
    if (p == null)    panic("vsmngr_alloc_vaddr(): no enough memory");
    return p;
}

/**
 * @brief request some free virtual pages
 * 
 * @param pcb the thread pcb to be request
 * @param amount request page amounts
 * @return a virtual address followed serveral pages available;
 * null if failure occured
 */
void *
vir_alloc_pages(pcb_t *pcb, uint32_t amount) {
    if (pcb == null)    panic("vir_alloc_pages(): invalid parameter");
    if (amount == 0)    panic("vir_alloc_pages(): cannot request 0 page");

    // traversal the virtual space
    const uint32_t ADDR_BASE = pcb == get_hoo_pcb() ? 0 : pcb->break_;
    const uint32_t ADDR_END = pcb == get_hoo_pcb() ?
        MAX_VSPACE_HOO : KERN_HIGH_MAPPING;
    uint32_t last_end = ADDR_BASE, ret = 0;
    vspace_t *worker = &pcb->vmngr_.head_;

    while (worker) {

        if (worker->next_ && (((worker->next_->begin_ - last_end) / PGSIZE) < amount)) {
            // there is not enough space from here to next interval

            last_end = worker->next_->end_;
            worker = worker->next_;
        } else {
            // there is enough space; or there is no next interval
            if (!worker->next_ && last_end + amount * PGSIZE >= ADDR_END)
                panic("vir_alloc_pages(): no enough space");

            vspace_t *temp = worker;
            if (list_isempty(&worker->list_)) {
                temp = vsmngr_alloc_vspace(&pcb->vmngr_);
                vspace_set(temp, null, last_end, last_end, null);
            }
            node_t   *node_free = vsmngr_alloc_node(&pcb->vmngr_);
            vaddr_t  *vaddr_free = vsmngr_alloc_vaddr(&pcb->vmngr_);

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
 * @brief metadata vspace releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata vspace object
 */
static void
vsmngr_release_vspace(vsmngr_t *mngr, vspace_t *vs) {
    fmtlist_release(mngr->vspace_, vs, sizeof(vspace_t));
}

/**
 * @brief metadata node releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata node object
 */
static void
vsmngr_release_node(vsmngr_t *mngr, node_t *node) {
    fmtlist_release(mngr->node_, node, sizeof(node_t));
}

/**
 * @brief metadata vaddr releasing
 * 
 * @param mngr `vsmngr` object
 * @param vs metadata vaddr object
 */
static void
vsmngr_release_vaddr(vsmngr_t *mngr, vaddr_t *vaddr) {
    fmtlist_release(mngr->vaddr_, vaddr, sizeof(vaddr_t));
}

/**
 * @brief release some virtual addresses
 * 
 * @param pcb the thread pcb to be request
 * @param va the beginning virtual address
 */
void
vir_release_pages(pcb_t *pcb, void *va) {
    if (pcb == null)    panic("vir_release_pages(): paremeter invalid");

    // search the `vspace` list according to virtual address
    vspace_t *worker_vs = &pcb->vmngr_.head_, *prev_vs = null;
    while (worker_vs) {
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
    idx_t i = 1;
    vspace_t *new_vs = null;
    do {
        worker_node = list_find(&worker_vs->list_, i);
        if (worker_node) {
            if ((uint32_t)va == ((vaddr_t*)(worker_node->data_))->va_) {

                lct = (i == 1) ? LCT_BEGIN : ((i == worker_vs->list_.size_)
                    ? LCT_END : LCT_MIDDLE);
                list_remove(&worker_vs->list_, i);

                if (lct == LCT_MIDDLE) {
                    // it is the middle node
                    new_vs = vsmngr_alloc_vspace(&pcb->vmngr_);
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
    } while (worker_node);
    // previously, the virtual address is belong to the current list,
    //   so it must exist in current list.
    if (worker_node == null)    panic("vir_release_pages(): bug");

    // release pages
    uint32_t pages_amount = ((vaddr_t *)(worker_node->data_))->length_;
    for (uint32_t i = 0; i < pages_amount; ++i) {
        uint32_t va = ((vaddr_t *)(worker_node->data_))->va_;
        phy_release_vpage(pcb, (void *)(va + i * PGSIZE));
    }

    // reclaim `vspace` list
    if (list_isempty(&worker_vs->list_)) {
        vspace_append(prev_vs, worker_vs->next_);
        vsmngr_release_vspace(&pcb->vmngr_, worker_vs);
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
    vsmngr_release_vaddr(&pcb->vmngr_, worker_node->data_);
    vsmngr_release_node(&pcb->vmngr_, worker_node);
}

/**
 * @brief release a physical page
 * 
 * @param page_vir_addr the corresponding virtual address
 */
void
phy_release_vpage(pcb_t *pcb, void *page_vir_addr) {
    if (pcb == null)    panic("phy_release_vpage(): null pointer");
    if (pcb == get_hoo_pcb()
        && (uint32_t)page_vir_addr < KERN_HIGH_MAPPING + MM_BASE)
        panic("phy_release_vpage(): cannot release kernel virtual space");

    bzero(page_vir_addr, PGSIZE);
    pgelem_t *pde = (pgelem_t *)GET_PDE(page_vir_addr);
    phy_release_page((void *)PG(*pde));
}

/**
 * @brief release the virtual space
 * 
 * @param pcb the thread
 */
void
release_vspace(pcb_t *pcb) {
    if (pcb == null)    panic("release_vspace(): null pointer");

    vspace_t *worker_vs = pcb->vmngr_.head_.next_;
    while (worker_vs) {
        node_t *worker_node = worker_vs->list_.null_.next_;
        while (worker_node) {
            vaddr_t *worker_vaddr = (vaddr_t *)worker_node->data_;
            if (worker_vaddr == null)
                panic("release_vspace(): bug");

            // release pages except metadata (va = 0)
            uint32_t pages_amount = ((vaddr_t *)(worker_node->data_))->length_;
            uint32_t va = ((vaddr_t *)(worker_node->data_))->va_;
            node_t *worker_node_next = worker_node->next_;
            if (va != 0) {
                for (uint32_t i = 0; i < pages_amount; ++i)
                    phy_release_vpage(pcb, (void *)(va + i * PGSIZE));

                // reclaim the metadata
                vsmngr_release_vaddr(&pcb->vmngr_, worker_node->data_);
                vsmngr_release_node(&pcb->vmngr_, worker_node);
            }
            worker_node = worker_node_next;
        } // end while(node)

        vspace_t *worker_vs_next = worker_vs->next_;
        if (list_isempty(&worker_vs->list_))
            vsmngr_release_vspace(&pcb->vmngr_, worker_vs);
        worker_vs = worker_vs_next;
    } // end while(vspace)

    node_set(&pcb->vmngr_.head_.list_.null_, null, null);
}
