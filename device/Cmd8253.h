/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_CMD8253_H__
#define __DEVICE_CMD8253_H__

#define CHANNEL0        0x40
#define COMMAND_8253    0x43
#define FREQUENCY_DEF   1193182

/**
 * @brief Mode/Command register, Select channel field
 */
typedef enum select_channel {
    SC_CHANNEL0 = 0,                                        // channel 0
    SC_CHANNEL1 = 64,                                       // channel 1
    SC_CHANNEL2 = 128                                       // channel 2
} sc_t;

/**
 * @brief Mode/Command register, Access mode field
 */
typedef enum access_mode {
    LATCH = 0,                                              // latch count value command
    LOWBYTE = 16,                                           // low byte only
    HIGHBYTE = 32,                                          // high byte only
    LOWHIGHBYTE = 48                                        // low byte first, then high
} am_t;

/**
 * @brief Mode/Command register, Operating mode field
 */
typedef enum operating_mode {
    M0 = 0,                                                 // interrupt on terminal count
    M1 = 2,                                                 // hardware re-triggerable one-shot
    M2 = 4,                                                 // rate generator
    M3 = 6,                                                 // square wave generator
    M4 = 8,                                                 // software triggered strobe
    M5 = 10,                                                // hardware triggered strobe
    M2_2 = 12,                                              // rate generator, same as M2
    M3_2 = 14                                               // square wave generator, same as M3
} om_t;

/**
 * @brief Mode/Command register, Bcd mode field
 */
typedef enum bcd_mode {
    BINARY = 0,                                             // binary format
    BCD                                                     // BCD format
} bm_t;

#endif
