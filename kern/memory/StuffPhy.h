/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEMORY_STUFFPHY_H__
#define __KERN_MEMORY_STUFFPHY_H__

#include "types.h"

/**
 * @brief definition of ARDS structure
 */
typedef struct {
    uint32_t base_low_;
    uint32_t base_hig_;
    uint32_t length_low_;
    uint32_t length_hig_;
    uint32_t type_;
} __attribute__((packed)) ards_t;

/**
 * @brief ARDS type enum
 */
typedef enum {
    ardstype_os = 1,                                        // OS can use
    ardstype_arch                                           // arch reserves
} ardstype_t;

/**
 * @brief node used for linking all memory
 */
typedef struct phy_page {
    uint8_t *pgaddr;                                        // page address
    struct phy_page *next;                                  // indicate the next node
} ppg_t;

/**
 * @brief 
 */
typedef struct phy_page_range {
    uint8_t *ppg_base;                                      // the first page
    uint8_t *ppg_end;                                       // the last page
    size_t pg_amount;                                       // pages amount
} ppg_range_t;

#endif
