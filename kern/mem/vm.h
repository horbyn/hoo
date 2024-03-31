/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VM_H__
#define __KERN_MEM_VM_H__

#include "vaddr.h"
#include "kern/conf/page.h"
#include "kern/driver/io.h"
#include "kern/mem/pm.h"
#include "kern/module/idle.h"
#include "kern/units/lib.h"
#include "kern/units/spinlock.h"
#include "kern/sched/tasks.h"

// the maximum value of idle thread virtual space -- the cause subtracted from
// 4-MB is that the last entry of its page directory table is not allowed to use
#define MAX_VSPACE_IDLE     (0xfffff000 - MB4 + 0x1000)

/**
 * @brief metadata provider of the virtual memory module;
 * access this structure by macro or functions because
 * I expect the members are invisiable outside
 */
typedef struct vm_slot {
    // to specify which thread
    tid_t    tid_;
    // the reference count of all the metadata
    uint32_t vs_cnt_, node_cnt_, vaddr_cnt_;
    // the metadata
    list_t   *vs_free_, *node_free_, *vaddr_free_;
} vmslot_t;

/**
 * @brief get the total number of elements of specific metadata list
 * @param type type
 */
#define GET_FREE_LIST_LEN(type) \
    (((PGSIZE) - (sizeof(list_t))) / ((sizeof(node_t) + sizeof(type))))

/*
* page formatting as the following:
* 
* ┌────────┬────────┬──────────┬─────────┬────────┬──────────┬────┐
* │ list_t │ node_t │   xx_t   │         │ node_t │   xx_t   │    │
* │        │                   │   ...   │                   │ .. │
* │ (head) │      (elem 1)     │         │      (elem x)     │    │
* └────────┴────────┴──────────┴─────────┴────────┴──────────┴────┘
* va
*/

/**
 * @brief initialize the free list for specific type and list
 * 
 * @param ptr_list the list for the third parameter
 * @param ptr_va virtual address
 * @param type one of these type -- `vspace_t`, `node_t` and `vaddr_t`
 */
#define GET_FREE_LIST(ptr_list, ptr_va, type)   \
    do { \
        if (ptr_list)    panic("GET_FREE_LIST(): list has existed"); \
        pcb_t *cur_pcb = get_current_pcb(); \
        if (cur_pcb != __pcb_idle) \
            panic("GET_FREE_LIST(): not allow other kernel threads to call"); \
        void *pa = phy_alloc_page(); \
        set_mapping(cur_pcb->pdir_va_, (uint32_t)ptr_va, (uint32_t)pa); \
        bzero(ptr_va, PGSIZE); \
        uint8_t *p = (uint8_t *)ptr_va; \
        uint32_t head_sz = sizeof(list_t); \
        uint32_t elem_sz = sizeof(node_t) + sizeof(type); \
        uint32_t elem_cnt = (PGSIZE - head_sz) / elem_sz; \
        ptr_list = (list_t *)p; \
        list_init(ptr_list, true); \
        p += head_sz; \
        for (uint32_t i = 0; i < elem_cnt; ++i) { \
            node_t *n = (node_t *)p; \
            type *type_elem = (type *)(p + sizeof(node_t)); \
            p += elem_sz; \
            node_set(n, type_elem, (node_t *)p); \
            list_insert(ptr_list, n, LSIDX_AFTAIL(ptr_list)); \
        } \
    } while (0)

/**
 * @brief check whether the vspace list is empty
 */
#define ISEMPTY_LIST_VSPACE(ptr_slot) \
    ((ptr_slot->vs_free_) == null)

/**
 * @brief check whether the node list is empty
 */
#define ISEMPTY_LIST_NODE(ptr_slot) \
    ((ptr_slot->node_free_) == null)

/**
 * @brief check whether the vaddr list is empty
 */
#define ISEMPTY_LIST_VADDR(ptr_slot) \
    ((ptr_slot->vaddr_free_) == null)

/**
 * @brief reclaim the free list of specific type
 */
#define RECLAIM_METADATA(ptr_slot_list, ptr_free, type) \
    do { \
        if (ptr_free) { \
            if (ptr_slot_list == null)    panic("vmslot_reclaim_" #type "(): invalid parameter"); \
            uint8_t *p = (uint8_t *)ptr_free; \
            p -= sizeof(node_t); \
            bzero(p, sizeof(node_t) + sizeof(type)); \
            ((node_t *)p)->data_ = ptr_free; \
            list_insert(ptr_slot_list, (node_t *)p, 1); \
        } \
    } while(0)

void vmslot_set_tid(vmslot_t *slot, tid_t tid);
void vmslot_set_vspace(vmslot_t *slot, void *va);
void vmslot_set_node(vmslot_t   *slot, void *va);
void vmslot_set_vaddr(vmslot_t  *slot, void *va);
vmslot_t *vmslot_get(tid_t tid);
vspace_t *vmslot_get_vspace(vmslot_t *slot);
node_t   *vmslot_get_node(vmslot_t *slot);
vaddr_t  *vmslot_get_vaddr(vmslot_t *slot);
void vmslot_reclaim_vspace(vmslot_t *slot, vspace_t *vs);
void vmslot_reclaim_node(vmslot_t *slot, node_t *node);
void vmslot_reclaim_vaddr(vmslot_t *slot, vaddr_t *vaddr);
void init_virmm_system(void);
void *vir_alloc_pages(tid_t tid, vspace_t *vs, uint32_t amount);
void vir_release_pages(tid_t tid, vspace_t *vs, void *va);

#endif
