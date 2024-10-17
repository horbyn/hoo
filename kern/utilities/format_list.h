/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_FORMAT_LIST_H__
#define __KERN_UTILITIES_FORMAT_LIST_H__

#include "list.h"

/**
 * @brief formatting list definition
 */
typedef struct format_list {
    // different type has different size
    uint32_t type_size_;
    list_t   list_;
} fmtlist_t;

void fmtlist_init(fmtlist_t *fmtlist, uint32_t type_size, bool is_cycle);
void *fmtlist_alloc(fmtlist_t *fmtlist);
void fmtlist_release(fmtlist_t *fmtlist, void *elem, uint32_t elem_size);

#endif
