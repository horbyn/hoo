#include "8259a.h"

/**
 * @brief 设置 ICW1 命令
 * 
 * @param cmd ICW1 命令
 */
void
set_icw1(uint8_t cmd) {
    // ICW1 需要设置为以下格式
    // 7  6 5 4  3   2  1    0
    // ┌─┼─┼─┼─┼────┼─┼────┼───┐
    // │0│0│0│1│LTIM│0│SNGL│IC4│
    // └─┼─┼─┼─┼────┼─┼────┼───┘

    cmd |= 0x10;
    outb(cmd, MAS_EVEN);
    outb(cmd, SLA_EVEN);
}

/**
 * @brief 设置 ICW2 命令
 * 
 * @param master master 的矢量偏移
 * @param slave  slave 的矢量偏移
 */
void
set_icw2(uint8_t master, uint8_t slave) {
    // ICW2
    //  7  6   5  4  3  2   1   0
    // ┌──┼──┼──┼──┼──┼───┼───┼───┐
    // │T7│T6│T5│T4│T3│ 0 │ 0 │ 0 │
    // └──┼──┼──┼──┼──┼───┼───┼───┘

    master &= 0xf8;
    slave &= 0xf8;
    outb(master, MAS_EVEN + 1);
    outb(slave, SLA_EVEN + 1);
}

/**
 * @brief 设置 ICW3 命令
 * 
 * @param irq_pin 指出从片连接到主片的引脚
 */
void
set_icw3(uint8_t irq_pin) {
    // ICW3-master
    //  7  6   5  4  3  2  1  0
    // ┌──┼──┼──┼──┼──┼──┼──┼──┐
    // │S7│S6│S5│S4│S3│S2│S1│S0│
    // └──┼──┼──┼──┼──┼──┼──┼──┘
    //
    // ICW3-slave
    //  7 6 5 4 3  2   1   0
    // ┌─┼─┼─┼─┼─┼───┼───┼───┐
    // │0│0│0│0│0│ID2│ID1│ID0│
    // └─┼─┼─┼─┼─┼───┼───┼───┘

    outb(1<<irq_pin, MAS_EVEN + 1);
    outb(irq_pin, SLA_EVEN + 1);
}

/**
 * @brief 设置 ICW4 命令
 * 
 * @param cmd ICW4 命令
 */
void
set_icw4(uint8_t cmd) {
    // ICW4
    //  7 6 5   4   3   2   1   0
    // ┌─┼─┼─┼────┼───┼───┼────┼─┐
    // │0│0│0│SFNM│BUF│M/S│AEOI│1│
    // └─┼─┼─┼────┼───┼───┼────┼─┘

    cmd |= 0x01;
    outb(cmd, MAS_EVEN + 1);
    outb(cmd, SLA_EVEN + 1);
}

/**
 * @brief 禁用 OCW1 掩码
 * 
 * @param irq_pin
 */
void
disable_mask_ocw1(uint8_t irq_pin) {
    uint16_t port;
    uint8_t value;

    if(irq_pin < 8) {
        port = MAS_EVEN + 1;
    } else {
        port = SLA_EVEN + 1;
        irq_pin -= 8;
    }
    value = inb(port) | (1 << irq_pin);
    outb(value, port);
}

/**
 * @brief 启用 OCW1 掩码
 * 
 * @param irq_pin
 */
void
enable_mask_ocw1(uint8_t irq_pin) {
    uint16_t port;
    uint8_t value;

    if(irq_pin < 8) {
        port = MAS_EVEN + 1;
    } else {
        port = SLA_EVEN + 1;
        irq_pin -= 8;
    }
    value = inb(port) & ~(1 << irq_pin);
    outb(value, port);
}
