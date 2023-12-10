/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "debug.h"

/**
 * @brief display some messages in collapse
 * 
 * @param msg 
 */
void
panic(const char *msg) {
#ifdef DEBUG
    if (msg)    kprintf(msg);
#else
    (void)msg;
#endif

    __asm__ __volatile__ ("cli\n\thlt");
}
