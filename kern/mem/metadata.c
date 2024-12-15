/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "metadata.h"
#include "format_list.h"
#include "kern/utilities/spinlock.h"

static fmtlist_t *__vs = 0;
static fmtlist_t *__node = 0;
static fmtlist_t *__vaddr = 0;
static spinlock_t __slvs, __slata, __slva;

/**
 * @brief initialize metadata
 */
void
init_metadata() {
    __vs = null;
    __node = null;
    __vaddr = null;
    spinlock_init(&__slvs);
    spinlock_init(&__slata);
    spinlock_init(&__slva);
}

/**
 * @brief metadata vspace allocation
 */
vspace_t *
vspace_alloc() {
    vspace_t *tmp = null;
    wait(&__slvs);
    tmp = (vspace_t *)fmtlist_alloc(&__vs, sizeof(vspace_t));
    signal(&__slvs);
    return tmp;
}

/**
 * @brief metadata node allocation
 */
node_t *
node_alloc() {
    node_t *tmp = null;
    wait(&__slata);
    tmp = (node_t *)fmtlist_alloc(&__node, sizeof(node_t));
    signal(&__slata);
    return tmp;
}

/**
 * @brief metadata vaddr allocation
 */
vaddr_t *
vaddr_alloc() {
    vaddr_t *tmp = null;
    wait(&__slva);
    tmp = (vaddr_t *)fmtlist_alloc(&__vaddr, sizeof(vaddr_t));
    signal(&__slva);
    return tmp;
}

/**
 * @brief metadata vspace releasing
 * 
 * @param vs metadata vspace object
 */
void
vspace_release(vspace_t *vs) {
    wait(&__slvs);
    fmtlist_release(&__vs, vs, sizeof(vspace_t));
    signal(&__slvs);
}

/**
 * @brief metadata node releasing
 * 
 * @param vs metadata node object
 */
void
node_release(node_t *node) {
    wait(&__slata);
    fmtlist_release(&__node, node, sizeof(node_t));
    signal(&__slata);
}

/**
 * @brief metadata vaddr releasing
 * 
 * @param vs metadata vaddr object
 */
void
vaddr_release(vaddr_t *vaddr) {
    wait(&__slva);
    fmtlist_release(&__vaddr, vaddr, sizeof(vaddr_t));
    signal(&__slva);
}
