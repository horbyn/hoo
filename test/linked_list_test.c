/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "linked_list_test.h"

/**
 * @brief print the list from beginning to end
 * 
 * @param list the list
 * @param amount the amount for the elements to be printed
 */
static void
list_print(list_t *list, uint32_t amount) {
    node_t *p = list->null_.next_;
    while (amount) {
        if (p != &list->null_) {
            kprintf("%d -> ", *((uint32_t *)p->data_));
            --amount;
        }

        p = p->next_;
    }
    kprintf("\n");
}

/**
 * @brief test insertion
 */
static void
test_insert(void) {
    kprintf("common insertion\n");

    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list, false);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, list.size_ + 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, list.size_ + 1);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, list.size_ + 1);

    list_print(&list, list.size_);
}

/**
 * @brief test remove
 */
static void
test_remove(void) {
    kprintf("common removal\n");

    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list, false);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, list.size_ + 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, list.size_ + 1);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, list.size_ + 1);

    list_remove(&list, 2);

    list_print(&list, list.size_);
}

/**
 * @brief test insertion of cycle linked-list
 */
static void
test_cycle_insert(void) {
    kprintf("cycle insertion\n");

    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list, true);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, list.size_ + 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, list.size_ + 1);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, list.size_ + 1);

    list_print(&list, list.size_ * 3);
}

/**
 * @brief test remove
 */
static void
test_cycle_remove(void) {
    kprintf("cycle removal\n");

    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list, true);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, list.size_ + 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, list.size_ + 1);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, list.size_ + 1);

    list_remove(&list, 1);
    list_print(&list, list.size_ * 3);
    list_remove(&list, 1);
    list_print(&list, list.size_ * 3);
    list_remove(&list, 1);
    list_print(&list, list.size_ * 3);
}

/**
 * @brief test linked list structur
 */
void
test_linked_list(void) {
    clear_screen();

    test_insert();
    test_remove();
    test_cycle_insert();
    test_cycle_remove();
}
