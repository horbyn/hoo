/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_TSS_H__
#define __CONF_TSS_H__

#include "kern/types.h"

/**
 * @brief 32-bit TSS format
 */
typedef struct Tss {
    uint32_t prev_tss_;                                     // previous task
    uint32_t esp0_;                                         // ring0 stack
    uint32_t ss0_;                                          // ring0 stack segment
    uint32_t esp1_;                                         // ring1 stack
    uint32_t ss1_;                                          // ring1 stack segment
    uint32_t esp2_;                                         // ring2 stack
    uint32_t ss2_;                                          // ring2 stack segment
    uint32_t cr3_;                                          // page dir table base
    uint32_t eip_;
    uint32_t eflags_;
    uint32_t eax_;
    uint32_t ecx_;
    uint32_t edx_;
    uint32_t ebx_;
    uint32_t esp_;
    uint32_t ebp_;
    uint32_t esi_;
    uint32_t edi_;
    uint32_t es_;
    uint32_t cs_;
    uint32_t ss_;
    uint32_t ds_;
    uint32_t fs_;
    uint32_t gs_;
    uint32_t ldt_sel_;                                      // ldt segment selector
    uint32_t iomap_;                                        // io map base
} __attribute__ ((packed)) Tss_t;

#endif
