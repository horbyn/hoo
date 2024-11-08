/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_FORMAT_LIST_H__
#define __KERN_MEM_FORMAT_LIST_H__

#include "kern/utilities/list.h"

/**
 * @brief formatting list definition
 */
typedef struct format_list {
    // the size of the specific type
    uint32_t           type_size_;
    // list
    list_t             list_;
    // first element offset
    uint32_t           first_off_;
    // capacity
    uint32_t           capacity_;
    struct format_list *next_;
} fmtlist_t;

void *fmtlist_alloc(fmtlist_t **fmtlist, uint32_t size);
bool fmtlist_release(fmtlist_t **fmtlist, void *elem, uint32_t elem_size);

#endif
