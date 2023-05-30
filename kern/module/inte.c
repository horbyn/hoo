/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "inte.h"

/**
 * @brief interrupt initialization
 */
void
init_interrupt(void) {
    init_isr_idt();
}
