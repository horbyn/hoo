/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sched.h"

/**
 * @brief kernel initializes the tasks system
 */
void
kinit_tasks_system(void) {
    init_tasks_system();
}

/**
 * @brief create idle thread
 */
void
kinit_idle_thread(void) {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack
    __pcb_idle = (pcb_t *)STACK_BOOT_BOTTOM;
    tid_t idle_tid = allocate_tid();
    pcb_set(__pcb_idle, null, (uint32_t *)STACK_BOOT_TOP, idle_tid,
        __pgdir_idle, TIMETICKS);
    queue_t *running_queue = get_idle_running_queue();
    if (running_queue == null)    panic("kinit_idle_thread(): bug");
    static node_t n;
    node_set(&n, __pcb_idle, null);
    queue_push(running_queue, &n, TAIL);

    // setup virtual space
    vmslot_t *idle_slot = vmslot_get(idle_tid);
    vmslot_set_tid(idle_slot, idle_tid);
    void *va_vspace = (void *)(KERN_HIGH_MAPPING + MM_BASE);
    vmslot_set_vspace(idle_slot, va_vspace);
    void *va_node = (void *)((uint32_t)va_vspace + PGSIZE);
    vmslot_set_node(idle_slot, va_node);
    void *va_vaddr = (void *)((uint32_t)va_node + PGSIZE);
    vmslot_set_vaddr(idle_slot, va_vaddr);

    // setup va space: [0, KERN_HIGH_MAPPING + MM_BASE)
    vaddr_t *free_vaddr = vmslot_get_vaddr(idle_slot);
    vaddr_set(free_vaddr, 0, (KERN_HIGH_MAPPING + MM_BASE) / PGSIZE);
    node_t *free_node = vmslot_get_node(idle_slot);
    node_set(free_node, free_vaddr, null);
    vspace_t *free_vspace = vmslot_get_vspace(idle_slot);
    list_insert(&free_vspace->list_, free_node, 1);

    // setup va space: [`va_vspace`, `va_vspace` + PGSIZE)
    free_vaddr = vmslot_get_vaddr(idle_slot);
    vaddr_set(free_vaddr, (uint32_t)va_vspace, 1);
    free_node = vmslot_get_node(idle_slot);
    node_set(free_node, free_vaddr, null);
    list_insert(&free_vspace->list_, free_node, 1);

    // setup va space: [`va_node`, `va_node` + PGSIZE)
    free_vaddr = vmslot_get_vaddr(idle_slot);
    vaddr_set(free_vaddr, (uint32_t)va_node, 1);
    free_node = vmslot_get_node(idle_slot);
    node_set(free_node, free_vaddr, null);
    list_insert(&free_vspace->list_, free_node, 1);

    // setup va space: [`va_vaddr`, `va_vaddr` + PGSIZE)
    free_vaddr = vmslot_get_vaddr(idle_slot);
    vaddr_set(free_vaddr, (uint32_t)va_vaddr, 1);
    free_node = vmslot_get_node(idle_slot);
    node_set(free_node, free_vaddr, null);
    list_insert(&free_vspace->list_, free_node, 1);

    vspace_set(free_vspace, &free_vspace->list_, 0, (uint32_t)va_vaddr + PGSIZE,
        null);
    vspace_append(&__pcb_idle->vspace_, free_vspace);
}
