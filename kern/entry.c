int entry(void) {
    __asm__("movl %cs, %eax\r\n"
            "nop\r\n"
            "nop\r\n"
            "movl $0x9594, %eax");

    // should not return
    return 0;
}