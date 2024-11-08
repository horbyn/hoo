/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "metadata.h"
#include "format_list.h"

static fmtlist_t *__vs = 0;
static fmtlist_t *__node = 0;
static fmtlist_t *__vaddr = 0;

/**
 * @brief initialize metadata
 */
void
init_metadata() {
    __vs = null;
    __node = null;
    __vaddr = null;
}

/**
 * @brief metadata vspace allocation
 */
vspace_t *
vspace_alloc() {
    return (vspace_t *)fmtlist_alloc(&__vs, sizeof(vspace_t));
}

/**
 * @brief metadata node allocation
 */
node_t *
node_alloc() {
    return (node_t *)fmtlist_alloc(&__node, sizeof(node_t));
}

/**
 * @brief metadata vaddr allocation
 */
vaddr_t *
vaddr_alloc() {
    return (vaddr_t *)fmtlist_alloc(&__vaddr, sizeof(vaddr_t));
}

/**
 * @brief metadata vspace releasing
 * 
 * @param vs metadata vspace object
 */
void
vspace_release(vspace_t *vs) {
    fmtlist_release(&__vs, vs, sizeof(vspace_t));
}

/**
 * @brief metadata node releasing
 * 
 * @param vs metadata node object
 */
void
node_release(node_t *node) {
    fmtlist_release(&__node, node, sizeof(node_t));
}

/**
 * @brief metadata vaddr releasing
 * 
 * @param vs metadata vaddr object
 */
void
vaddr_release(vaddr_t *vaddr) {
    fmtlist_release(&__vaddr, vaddr, sizeof(vaddr_t));
}
