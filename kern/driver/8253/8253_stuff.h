#pragma once
#ifndef __KERN_DRIVER_8253_8253_STUFF_H__
#define __KERN_DRIVER_8253_8253_STUFF_H__

#define CHANNEL0        0x40
#define COMMAND_8253    0x43
#define FREQUENCY_DEF   1193182

/**
 * @brief Mode/Command 寄存器, Select channel 字段
 */
typedef enum select_channel {
    // 通道 0
    SC_CHANNEL0 = 0,
    // 通道 1
    SC_CHANNEL1 = 64,
    // 通道 2
    SC_CHANNEL2 = 128
} sc_t;

/**
 * @brief Mode/Command 寄存器, Access mode 字段
 */
typedef enum access_mode {
    // 倒计时命令
    LATCH = 0,
    // 只访问低字节
    LOWBYTE = 16,
    // 只访问高字节
    HIGHBYTE = 32,
    // 先访问高字节再访问低字节
    LOWHIGHBYTE = 48
} am_t;

/**
 * @brief Mode/Command 寄存器, Operating mode 字段
 */
typedef enum operating_mode {
    // interrupt on terminal count
    M0 = 0,
    // hardware re-triggerable one-shot
    M1 = 2,
    // rate generator
    M2 = 4,
    // square wave generator
    M3 = 6,
    // software triggered strobe
    M4 = 8,
    // hardware triggered strobe
    M5 = 10,
    // rate generator, same as M2
    M2_2 = 12,
    // square wave generator, same as M3
    M3_2 = 14
} om_t;

/**
 * @brief Mode/Command 寄存器, Bcd mode 字段
 */
typedef enum bcd_mode {
    // 二进制格式
    BINARY = 0,
    // BCD 码格式
    BCD
} bm_t;

#endif
