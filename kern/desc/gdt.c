#include "gdt.h"
#include "kern/panic.h"
#include "kern/module/log.h"

/**
 * @brief 设置 gdt 段描述符
 * 
 * @param d     gdt 对象
 * @param limit 寻址最大值，单位要么 1B 要么 4KB
 * @param base  对应段的虚拟地址起始
 * @param a     一个属性位，最好设置为 0
 * @param rw    读写标识
 * @param dc    direction or conforming
 * @param exe   执行标识
 * @param sys   是否系统段
 * @param dpl   优先级
 * @param ps    存在标识
 * @param l     是否 LONG 模式
 * @param db    是否保护模式
 * @param g     粒度
 */
void
set_gdt(Desc_t *d, uint32_t limit, uint32_t base, uint8_t a,
uint8_t rw, uint8_t dc, uint8_t exe, uint8_t sys, uint8_t dpl,
uint8_t ps, uint8_t l, uint8_t db, uint8_t g) {
    if (d == null)    panic("set_gdt(): null pointer");

    d->limit_15_0_                          = (uint16_t)limit;
    d->limit_19_16_                         = (uint8_t)(limit >> 16);
    d->base_15_0_                           = (uint16_t)base;
    d->base_23_16_                          = (uint8_t)(base >> 16);
    d->base_31_24_                          = (uint8_t)(base >> 24);
    if (sys != 0) {
        // 非系统段
        d->access_bytes_.code_or_data_.a_   = a;
        d->access_bytes_.code_or_data_.rw_  = rw;
        d->access_bytes_.code_or_data_.dc_  = dc;
        d->access_bytes_.code_or_data_.e_   = exe;
        d->access_bytes_.code_or_data_.sys_ = 1;
        d->access_bytes_.code_or_data_.dpl_ = dpl;
        d->access_bytes_.code_or_data_.ps_  = ps;
    } else {
        // 系统段
        d->access_bytes_.tss_.type1_        = 1;
        d->access_bytes_.tss_.type2_        = rw;
        d->access_bytes_.tss_.type3_        = 0;
        d->access_bytes_.tss_.type4_        = 1;
        d->access_bytes_.tss_.sys_          = 0;
        d->access_bytes_.tss_.dpl_          = dpl;
        d->access_bytes_.tss_.ps_           = ps;
    }
    d->avl_                                 = 0;
    d->long_                                = l;
    d->db_                                  = db;
    d->g_                                   = g;
}

/**
 * @brief 设置 gdtr
 * 
 * @param gdtr     gdtr 对象
 * @param gdt      gdt 对象
 * @param gdt_size gdt 大小
 */
void set_gdtr(Gdtr_t *gdtr, Desc_t *gdt, uint32_t gdt_size) {
    if (gdtr == null)    panic("set_gdtr(): null pointer");

    gdtr->size_   = gdt_size;
    gdtr->linear_ = gdt;
    Gdtr_t gdtr_value;
    __asm__ ("lgdt (%1)\n\t"
        "sgdt %0" : "=m"(gdtr_value) :"r"(gdtr));

    klog_write("[DEBUG] gdt: 0x%x, gdtr: 0x%x\n", gdt, gdtr_value.linear_);
}
