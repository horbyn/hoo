#ifndef __KERN_INTE_H__
#define __KERN_INTE_H__

#include "types.h"
#include "x86.h"

#define PIC_MAS_EVEN    0x20
#define PIC_SLA_EVEN    0xa0
#define IDT_ENTRIES_NUM 256

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
	uint16_t    isr_low;
	uint16_t    selector;
	uint8_t     reserved;
	uint8_t     attributes;
	uint16_t    isr_high;
} __attribute__((packed)) idt_entry_t;

// IDTR structure
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

void init_pic(void);
void init_idt(void);

#endif
