/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "kern/kernel.h"
#include "kern/module/config.h"

void
entry(void) {
    /********************************
     * ignore the boot environment  *
     * we reset all registers here  *
     *                              *
     * the address is same as       *
     * `boot/kern_will_use.inc`     *
     ********************************/
    __asm__ ("\r\n"
    "movw $0x10,    %ax\r\n"
    "movw %ax,      %ds\r\n"
    "movw %ax,      %es\r\n"
    "movw %ax,      %fs\r\n"
    "movw %ax,      %gs\r\n"
    "movw %ax,      %ss\r\n"
    "movl $0x80000, %esp\r\n"
    "pushl $0x77ffc\r\n"                                    // setup DIED INSTRUCTION
    "pushl $0\r\n"                                          // setup calling convention
    "movl %esp,     %ebp");

    kernel_config();
    kernel_exec();

    /********************************
     * NEED NOT TO RETURN NORMALLY  *
     *                              *
     * But return is no problem     *
     * because it will return to    *
     * DIED instruction             *
     ********************************/
}
