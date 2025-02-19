#include "vspace.h"
#include "kern/panic.h"
#include "user/lib.h"
#include "user/lib.h"
#include "kern/module/io.h"

/**
 * @brief 填充 vaddr 对象
 * 
 * @param vaddr  vaddr 对象
 * @param addr   虚拟地址
 * @param length 页框数量
 */
void
vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length) {
    if (vaddr == null)    panic("vaddr_set(): null pointer");
    vaddr->va_ = addr;
    vaddr->length_ = length;
}

/**
 * @brief 填充 vspace 对象
 * 
 * @param vs    vspace 对象
 * @param ls    链表
 * @param begin 区间起始地址
 * @param end   区间结束地址
 * @param next  下一个 vspace 对象
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
