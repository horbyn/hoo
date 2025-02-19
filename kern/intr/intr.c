#include "intr.h"
#include "kern/desc/desc.h"
#include "kern/panic.h"

/**
 * @brief 设置 IDT
 * 
 * @param idt  IDT 对象
 * @param dpl  描述符特权级
 * @param gate 门描述符
 * @param addr IDT 函数入口
 */
void
set_idt_entry(idt_t *idt, privilege_t dpl, gatedesc_t gate, uint32_t addr) {
    if (idt == null)    panic("set_idt_entry(): null pointer");

    // 最高 4 字节是 `P/DPL/0`
    uint8_t attr = (1 << 7 | dpl << 5 | (uint8_t)gate);

    idt->isr_low_    = (uint16_t)addr;
    idt->selector_   = CS_SELECTOR_KERN;
    idt->reserved_   = 0;
    idt->attributes_ = attr;
    idt->isr_high_   = (uint16_t)(addr >> 16);
}

/**
 * @brief 设置 ISR
 * 
 * @param isr  ISR
 * @param addr ISR 函数入口
 */
void
set_isr_entry(isr_t *isr, isr_t addr) {
    if (isr == null)    panic("set_isr_entry(): null pointer");
    *isr = addr;
}
