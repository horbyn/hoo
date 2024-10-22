/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_BUCKET_H__
#define __KERN_MEM_BUCKET_H__

#include "format_list.h"

/**
 * @brief memory bucket manager
 */
typedef struct buckX_manager {
    uint32_t             size_;
    fmtlist_t            *chain_;
    struct buckX_manager *next_;
} buckx_mngr_t;

void buckmngr_init(buckx_mngr_t *mngr, uint32_t size, fmtlist_t *chain,
    buckx_mngr_t *next);

#endif
