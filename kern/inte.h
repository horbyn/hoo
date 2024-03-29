#ifndef __KERN_INTE_H__
#define __KERN_INTE_H__

#include "types.h"
#include "x86.h"
#include "disp.h"

#define PIC_MAS_EVEN    0x20
#define PIC_SLA_EVEN    0xa0
#define IDT_INTEL_NUM   33
#define IDT_ENTRIES_NUM 256

typedef void (*isr_t)(void);

extern uint32_t isr_part1[IDT_INTEL_NUM];
extern isr_t isr[IDT_ENTRIES_NUM];

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

void init_pic(void);
void init_interrupt(void);
void set_idt_entry(int, uint32_t);
void set_isr_entry(int, uint32_t);

// ====================================
// ============== isr =================
// ====================================

void info(uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t);
void isr_default(void);
void divide_error(void);		// #0
void timer(void);				// #32

#endif
