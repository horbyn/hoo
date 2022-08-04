int test2() {
    char *str3 = "test2";
    int t2 = 100;
    __asm__("movw $0x1234, %ax\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "movw $0x5678, %ax");

    return 0;
}