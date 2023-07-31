/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_INTER_STUFFINTE_H__
#define __KERN_INTER_STUFFINTE_H__

#include "types.h"
#include "x86.h"

#define IDT_NEED_TO_INIT_NUM	33							// 32 intel reserved; 1 is timer
#define IDT_ENTRIES_NUM 		256

typedef void (*isr_t)(void);

extern uint32_t isr_part1[IDT_NEED_TO_INIT_NUM];
extern isr_t __isr[IDT_ENTRIES_NUM];

// IDT entries
// 63           48 47 46 45 44 43     40 39    32
// ┌──────────────┼──┼─────┼──┼─────────┼────────┐
// │Offset 31..16 │P │ DPL │ 0│Gate Type│Reserved│
// └──────────────┴──┴─────┴──┴─────────┴────────┘
// 31               16 15                       0
// ┌──────────────────┼──────────────────────────┐
// │Segment Selector  │  Offset 15..0            │
// └──────────────────┴──────────────────────────┘

typedef struct {
	uint16_t    isr_low;	// will be loaded to EIP
	uint16_t    selector;	// will be loaded to CS when interrupt happened
	uint8_t     reserved;
	uint8_t     attributes;
	uint16_t    isr_high;	// will be loaded to EIP
} __attribute__((packed)) idt_entry_t;

// IDTR structure
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

#endif
