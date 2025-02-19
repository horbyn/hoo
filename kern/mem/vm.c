#include "vm.h"
#include "metadata.h"
#include "pm.h"
#include "vm_kern.h"
#include "kern/panic.h"
#include "kern/sched/pcb.h"

/**
 * @brief 将 next 追加到 cur 后面
 * 
 * @param cur  当前的 vspace 对象
 * @param next 下一个 vspace 对象
 */
static void
vspace_append(vspace_t *cur, vspace_t *next) {
    if (cur == null)    panic("vspace_append(): null pointer");
    if (next == null)    cur->next_ = null;
    else    cur->next_ = next;

    cur->end_ = next->begin_;
}

/**
 * @brief 初始化虚拟内存管理模块
 */
void
init_virmm_system() {
    init_kern_virmm_bitmap();
    init_metadata();
}

/**
 * @brief 分配虚拟地址
 * 
 * @param vspace vspace 对象
 * @param amount 需要请求的页框数量
 * @param begin  从哪个虚拟地址开始往后查找可用的虚拟地址
 * @retval 虚拟地址，此时其后的 amount 个页框也是可用的
 * @retval null，出错
 */
void *
vir_alloc_pages(vspace_t *vspace, uint32_t amount, uint32_t begin) {
    if (vspace == null)    panic("vir_alloc_pages(): null pointer");
    if (amount == 0)    panic("vir_alloc_pages(): cannot request 0 page");

    // 遍历虚拟地址空间
    const uint32_t MAX_END =
        begin < KERN_HIGH_MAPPING ? KERN_HIGH_MAPPING : KERN_METADATA;
    uint32_t last_end = begin, ret = 0;
    vspace_t *worker = vspace;

    while (worker != null) {

        if (worker->next_ != null
            && (((worker->next_->begin_ - last_end) / PGSIZE) < amount)) {
            // 从当前这个区间到下一个区间之间没有足够的空间以容纳 amount 个页框

            last_end = worker->next_->end_;
            worker = worker->next_;
        } else {
            // 要么有足够空间，要么后面已经没有区间了
            if (worker->next_ == null
                && last_end + amount * PGSIZE >= MAX_END)
                panic("vir_alloc_pages(): no enough space");

            vspace_t *temp = worker;
            if (list_isempty(&worker->list_)) {
                temp = vspace_alloc();
                vspace_set(temp, null, last_end, last_end, null);
            }
            node_t  *node_free = node_alloc();
            vaddr_t *vaddr_free = vaddr_alloc();

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
 * @brief 释放虚拟地址
 * 
 * @param vspace vspace 对象
 * @param va     要释放的虚拟地址
 * @param rel    是否释放虚拟地址对应的物理页
 */
void
vir_release_pages(vspace_t *vspace, void *va, bool rel) {
    if (vspace == null)    panic("vir_release_pages(): null pointer");

    // 根据虚拟地址查找 vspace 链表
    vspace_t *worker_vs = vspace, *prev_vs = null;
    while (worker_vs != null) {
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

    // 根据虚拟地址查找 vspace 链表中的结点
    node_t *worker_node = null;
    int i = 1;
    vspace_t *new_vs = null;
    do {
        worker_node = list_find(&worker_vs->list_, i);
        if (worker_node != null) {
            if ((uint32_t)va == ((vaddr_t*)(worker_node->data_))->va_) {

                lct = (i == 1) ? LCT_BEGIN : ((i == worker_vs->list_.size_)
                    ? LCT_END : LCT_MIDDLE);
                list_remove(&worker_vs->list_, i);

                if (lct == LCT_MIDDLE) {
                    // 处于链表的中间结点
                    new_vs = vspace_alloc();
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
    } while (worker_node != null);
    // 前面已经检查过虚拟地址是属于当前链表的，所以一定存在，不存在就是有 bug
    if (worker_node == null)    panic("vir_release_pages(): bug");

    // 释放物理页
    uint32_t pages_amount = ((vaddr_t *)(worker_node->data_))->length_;
    if (rel) {
        for (uint32_t i = 0; i < pages_amount; ++i) {
            uint32_t va = ((vaddr_t *)(worker_node->data_))->va_;
            phy_release_vpage((void *)(va + i * PGSIZE));
        }
    }

    // 释放 vspace 链表
    if (list_isempty(&worker_vs->list_)) {
        vspace_append(prev_vs, worker_vs->next_);
        vspace_release(worker_vs);
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

    // 释放 metadata
    node_release(worker_node);
    vaddr_release(worker_node->data_);
}

/**
 * @brief 释放 vspace 代表的线性空间
 * 
 * @param pcb 要释放 vspace 空间的线程 PCB
 */
void
release_vspace(pcb_t *pcb) {
    if (pcb == null)    panic("release_vspace(): null pointer");

    vspace_t *worker_vs = pcb->vmngr_.next_;
    while (worker_vs != null) {
        node_t *worker_node = worker_vs->list_.null_.next_;
        while (worker_node != null) {
            vaddr_t *worker_vaddr = (vaddr_t *)worker_node->data_;
            if (worker_vaddr == null)
                panic("release_vspace(): bug");

            // 除了 metadata 以外，释放所有页
            uint32_t pages_amount = ((vaddr_t *)(worker_node->data_))->length_;
            void  *va = (void *)((vaddr_t *)(worker_node->data_))->va_;
            node_t *worker_node_next = worker_node->next_;
            if (va != null) {
                for (uint32_t i = 0; i < pages_amount; ++i)
                    phy_release_vpage(va + i * PGSIZE);

                // 释放 metadata
                vaddr_release(worker_node->data_);
                node_release(worker_node);
            }
            worker_node = worker_node_next;
        } // end while(node)

        vspace_t *worker_vs_next = worker_vs->next_;
        if (list_isempty(&worker_vs->list_))
            vspace_release(worker_vs);
        worker_vs = worker_vs_next;
    } // end while(vspace)

    node_set(&pcb->vmngr_.list_.null_, null, null);
}
