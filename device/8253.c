/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "8253.h"

/**
 * @brief Set the 8253 chip command
 * 
 * @param sc select_channel enum
 * @param am access_mode enum
 * @param om operating_mode enum
 * @param bm bcd_mode enum
 */
void
set_command(sc_t sc, am_t am, om_t om, bm_t bm) {
    uint8_t cmd = sc | am | om | bm;
    outb(cmd, COMMAND_8253);
}

/**
 * @brief Set the counter initializer
 * 
 * @param frequency signal amount per second
 */
void
set_counter(size_t frequency) {
    uint16_t init = FREQUENCY_DEF / frequency;
    outb((uint8_t)init, CHANNEL0);
    outb((uint8_t)(init >> 8), CHANNEL0);
}
