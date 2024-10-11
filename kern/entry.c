/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/

void
entry(void) {
    /**********************************************************************
     * ignore the boot environment, because we reset all registers here   *
     *                                                                    *
     * the address is same as the setting in `boot/config_boot.h`         *
     **********************************************************************/
    __asm__ ("movw $0x10,    %ax\r\n"
             "movw %ax,      %ds\r\n"
             "movw %ax,      %es\r\n"
             "movw %ax,      %fs\r\n"
             "movw %ax,      %gs\r\n"
             "movw %ax,      %ss\r\n"
             "movl $0x80000, %esp\r\n"
             // setup DIED INSTRUCTION
             "pushl $0x77ffc\r\n"
             // setup calling convention
             "pushl $0\r\n"
             "movl %esp,     %ebp\r\n"
             "pushl $go\r\n"
             // goto the high address
             "ret\r\n"
             "go:\r\n");

    while (1);

    /**********************************************************************
     * NEED NOT TO RETURN NORMALLY                                        *
     *                                                                    *
     * But return is no problem because it will return to DIED instruction*
     **********************************************************************/
}
