#pragma once
#ifndef __BOOT_CONFIG_BOOT_H__
#define __BOOT_CONFIG_BOOT_H__

/*
 * **********************************************************************************
 * ********************* the following layout used for boot *************************
 *                                                                                  *
 * 0x77ffc          0x78000    0x79000    0x7a000 0x7a200  0x7a204  0x7a400 0x80000 *
 * ──────┼────────────────┼──────────┼──────────┼───────┼────────┼────────┼───────┤ *
 *       │DIED INSTRUCTION│ PD TABLE │ PG TABLE │  MBR  │ARDS NUM│  ARDS  │ STACK │ *
 * ──────┼────────────────┼──────────┼──────────┼───────┼────────┼────────┼───────┤ *
 *                                                                                  *
 * **********************************************************************************
 */

// 内核栈
//     上图栈布局是 0x80000，但为什么这个定义看起来栈顶是 0x70000？因为计算入栈时要
//     先减去 sp(esp)，所以如果 sp(esp) 是 0，然后入栈，那 ss:sp(esp) 的结果才是
//     0x7000:0xfffe(0xfffc)
//
//     这就是为什么定义栈顶为 0x7000
#define SEG_KSTACK          0x7000

// 这里设置一条 `jmp .` 指令
#define SEG_DIED            0x77f0
#define OFF_DIED            0xfc

// 记录 ARDS 结构体临时数据
#define SEG_ARDS            0x7a20
#define OFF_ARDS_CR         0
#define OFF_ARDS            4

// 记录引导阶段临时的 paging-structure
#define SEG_PDTABLE         0x7800
#define SEG_PGTABLE         0x7900

#define KERN_HIGH_MAPPING   0x80000000
#define PDE_HIGH_OFF        (KERN_HIGH_MAPPING >> 20)
#define PDE_LAST_OFF        (0xffc - PDE_HIGH_OFF)

// 内存起始
#define MM_BASE             0x100000

#endif
