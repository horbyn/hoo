/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "list.h"
#include "kern/panic.h"
#include "kern/x86.h"

/**
 * @brief list initialization
 * 
 * @param list list
 * @param cycle true if you want to create a cycle linked-list
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
 * @brief find the node of the list according to the index
 * (from 0 to size)
 * 
 * @param list list
 * @param idx index
 * @return the node to find
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
 * @brief insert the node of the list according to the index
 * (from 1 to size + 1)
 * 
 * @param list list
 * @param node node
 * @param idx index
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
 * @brief remove the node of the list according to the index
 * (from 1 to size)
 * 
 * @param list list
 * @param idx index
 * @return the removed node
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
 * @brief whether the specific list is empty
 * 
 * @param list list
 * @retval true: empty
 * @retval false: not empty
 */
bool
list_isempty(list_t *list) {
    if (list == null)    panic("list_isempty(): null pointer");
    return list->size_ == 0;
}
