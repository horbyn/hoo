/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_8253_CMD_8253_H__
#define __KERN_DRIVER_8253_CMD_8253_H__

#define CHANNEL0        0x40
#define COMMAND_8253    0x43
#define FREQUENCY_DEF   1193182

/**
 * @brief Mode/Command register, Select channel field
 */
typedef enum select_channel {
    // channel 0
    SC_CHANNEL0 = 0,
    // channel 1
    SC_CHANNEL1 = 64,
    // channel 2
    SC_CHANNEL2 = 128
} sc_t;

/**
 * @brief Mode/Command register, Access mode field
 */
typedef enum access_mode {
    // latch count value command
    LATCH = 0,
    // low byte only
    LOWBYTE = 16,
    // high byte only
    HIGHBYTE = 32,
    // low byte first, then high
    LOWHIGHBYTE = 48
} am_t;

/**
 * @brief Mode/Command register, Operating mode field
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
 * @brief Mode/Command register, Bcd mode field
 */
typedef enum bcd_mode {
    // binary format
    BINARY = 0,
    // BCD format
    BCD
} bm_t;

#endif
