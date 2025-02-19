#include "8253.h"

/**
 * @brief 设置 8253 芯片
 * 
 * @param sc 枚举值 select channel
 * @param am 枚举值 access mode
 * @param om 枚举值 operating mode
 * @param bm 枚举值 bcd mode
 */
void
set_command(sc_t sc, am_t am, om_t om, bm_t bm) {
    uint8_t cmd = sc | am | om | bm;
    outb(cmd, COMMAND_8253);
}

/**
 * @brief 初始化计数器
 * 
 * @param frequency 每秒的信号数量
 */
void
set_counter(uint32_t frequency) {
    uint16_t init = FREQUENCY_DEF / frequency;
    outb((uint8_t)init, CHANNEL0);
    outb((uint8_t)(init >> 8), CHANNEL0);
}
