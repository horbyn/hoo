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
 */
static void
list_print(list_t *list) {
    node_t *p = list->null_.next_;
    while (p) {
        kprintf("%d -> ", *((uint32_t *)p->data_));
        p = p->next_;
    }
    kprintf("\n");
}

/**
 * @brief test insertion
 */
static void
test_insert(void) {
    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, 2);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, 3);

    list_print(&list);
}

/**
 * @brief test remove
 */
static void
test_remove(void) {
    list_t list;
    uint32_t v1 = 1, v2 = 2, v3 = 3;

    list_init(&list);

    node_t node1;
    node1.data_ = &v1;
    node1.next_ = null;
    list_insert(&list, &node1, 1);

    node_t node2;
    node2.data_ = &v2;
    node2.next_ = null;
    list_insert(&list, &node2, 2);

    node_t node3;
    node3.data_ = &v3;
    node3.next_ = null;
    list_insert(&list, &node3, 3);

    list_remove(&list, 2);

    list_print(&list);
}

/**
 * @brief test linked list structur
 */
void
test_linked_list(void) {
    test_insert();
    test_remove();
}
