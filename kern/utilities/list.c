#include "list.h"
#include "kern/panic.h"
#include "kern/x86.h"

/**
 * @brief 链表初始化
 * 
 * @param list 链表
 * @param cycle 想要一个循环链表则设为 true
 */
void
list_init(list_t *list, bool cycle) {
    if (list == null)    panic("list_init(): null pointer");

    list->null_.data_ = null;
    if (cycle)    list->null_.next_ = &list->null_;
    else    list->null_.next_ = null;
    list->size_ = 0;
}

/**
 * @brief 链表按下标查找（从 0 到 size）
 * 
 * @param list 链表
 * @param idx  下标
 * @return 找到的结点
 */
node_t *
list_find(list_t *list, int idx) {
    if (list == null)    panic("list_find(): null pointer");
    if (idx == INVALID_INDEX || 0 > idx || idx > list->size_)
        panic("list_find(): invalid idx");

    node_t *p = &(list->null_);
    while (idx != 0 && idx--)    p = p->next_;
    return p;
}

/**
 * @brief 根据下标进行插入（从 1 到 size+1）
 * 
 * @param list 链表
 * @param node 插入的结点
 * @param idx  插入下标
 */
void
list_insert(list_t *list, node_t *node, int idx) {
    if (list == null || node == null)    panic("list_insert(): null pointer");
    if (idx == INVALID_INDEX || idx < 1 || idx > list->size_ + 1)
        panic("list_insert(): invalid idx");

    node_t *find = list_find(list, idx - 1);
    node->next_ = find->next_;
    find->next_ = node;
    list->size_++;
}

/**
 * @brief 根据链表下标进行删除（从 1 到 size）
 * 
 * @param list 链表
 * @param idx  下标
 * @return 已经删除的结点
 */
node_t *
list_remove(list_t *list, int idx) {
    if (list == null)    panic("list_remove(): null pointer");
    if (idx == INVALID_INDEX || idx < 1 || idx > list->size_)
        panic("list_remove(): invalid idx");

    node_t *find = list_find(list, idx - 1),
        *removed = find->next_;
    find->next_ = removed->next_;
    removed->next_ = null;
    list->size_--;
    return removed;
}

/**
 * @brief 判断链表空
 * 
 * @param list 链表
 * @retval true:  空
 * @retval false: 非空
 */
bool
list_isempty(list_t *list) {
    if (list == null)    panic("list_isempty(): null pointer");
    return list->size_ == 0;
}
