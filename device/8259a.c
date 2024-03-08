/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "8259a.h"

/**
 * @brief Set the icw1 command
 * 
 * @param cmd icw1 command
 */
void
set_icw1(uint8_t cmd) {
    // ICW1 need to set the following
    // 7  6 5 4  3   2  1    0
    // ┌─┼─┼─┼─┼────┼─┼────┼───┐
    // │0│0│0│1│LTIM│0│SNGL│IC4│
    // └─┼─┼─┼─┼────┼─┼────┼───┘

    cmd |= 0x10;
    outb(cmd, MAS_EVEN);
    outb(cmd, SLA_EVEN);
}

/**
 * @brief Set the icw2 command
 * 
 * @param master vector offset for master, which become master..master+7
 * @param slave  same for slave, slave..slave+7
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
 * @brief Set the icw3 command
 * 
 * @param irq_pin indicate the slave pin to connect to master
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
 * @brief Set the icw4 command
 * 
 * @param cmd icw4 command
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
 * @brief Disable the ocw1 mask
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
 * @brief Enable the ocw1 mask
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
    outb(port, value);
}
