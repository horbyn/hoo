#pragma once
#ifndef __KERN_DESC_DESC_H__
#define __KERN_DESC_DESC_H__

#include "kern/x86.h"

#define CS_SELECTOR_KERN    0x08
#define DS_SELECTOR_KERN    0x10
#define CS_SELECTOR_USER    0x1b
#define DS_SELECTOR_USER    0x23

/**
 * @brief Access Byte 定义
 */
typedef struct AccessByteNotsys {
    /*
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤
     * │P│DPL │S│E│DC│RW│A│
     * └─┴────┴─┴─┴──┴──┴─┘
     */

    // Access：最好保持为 0
    uint8_t a_   :1;
    // 可读或可写
    uint8_t rw_  :1;
    // direction or conforming
    uint8_t dc_  :1;
    // 可执行
    uint8_t e_   :1;
    // 系统段标识
    uint8_t sys_ :1;
    // 优先级
    uint8_t dpl_ :2;
    // 存在位标识
    uint8_t ps_  :1;
    
} __attribute__((packed)) AcsNotsys_t;

/**
 * @brief TSS 的 Access byte 定义
 */
typedef struct AccessByteTss {
    /*
     * TSS  使用
     * ┌─┬──┬─┬─┬─┬─┬─┬─┐
     * │7│ 6│5│4│3│2│1│0│
     * ├─┼──┴─┼─┼─┼─┼─┼─┤
     * │P│DPL │S│1│0│B│1│
     * └─┴────┴─┴─┴─┴─┴─┘
     */

    // 总是为 1
    uint8_t type1_ :1;
    // 如果是 1，则表示任务正在执行
    uint8_t type2_ :1;
    // 总是为 0
    uint8_t type3_ :1;
    // 总是为 1
    uint8_t type4_ :1;
    // 如果是系统段则为 0
    uint8_t sys_   :1;
    // 特权级
    uint8_t dpl_   :2;
    // 存在位标识
    uint8_t ps_    :1;
    
} __attribute__ ((packed)) AcsTss_t;

/**
 * @brief gdt 定义
 */
typedef struct Descriptor {
    uint16_t  limit_15_0_;
    uint16_t  base_15_0_;
    uint8_t   base_23_16_;
    /*
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤
     * │P│DPL │S│E│DC│RW│A│
     * └─┴────┴─┴─┴──┴──┴─┘
     */

    union {
        AcsNotsys_t  code_or_data_;
        AcsTss_t     tss_;
    } access_bytes_;
    uint8_t      limit_19_16_ :4;
    /*
     * ┌─┬──┬─┬────────┐
     * │3│ 2│1│       0│
     * ├─┼──┼─┼────────┤
     * │G│DB│L│Reserved│
     * └─┴──┴─┴────────┘
     */

    // 内核使用
    uint8_t   avl_  : 1;
    // LONG 模式设置为 1
    uint8_t   long_ : 1;
    // 保护模式设置为 1
    uint8_t   db_   : 1;
    // 粒度
    uint8_t   g_    : 1;
    uint8_t   base_31_24_;
} __attribute__ ((packed)) Desc_t;

/**
 * @brief GDTR 定义
 */
typedef struct GdtRegister {
    // gdt 大小
    uint16_t size_;
    // gdt 虚拟地址起始
    Desc_t *linear_;
} __attribute__ ((packed)) Gdtr_t;

/**
 * @brief 32 位 TSS 格式
 */
typedef struct Tss {
    // 前一个任务
    uint32_t prev_tss_;
    // ring0 栈顶
    uint32_t esp0_;
    // ring0 栈的段寄存器
    uint32_t ss0_;
    // ring1 栈顶
    uint32_t esp1_;
    // ring1 栈的段寄存器
    uint32_t ss1_;
    // ring2 栈顶
    uint32_t esp2_;
    // ring2 栈的段寄存器
    uint32_t ss2_;
    // 页目录表虚拟地址
    uint32_t cr3_;
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
    // ldt 段选择子
    uint32_t ldt_sel_;
    // io 映射虚拟地址
    uint32_t iomap_;
} __attribute__ ((packed)) tss_t;

#endif
