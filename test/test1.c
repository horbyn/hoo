__asm__(".code16gcc");

int test1() {
    char *str2 = "test1";
    int t1;
    __asm__("movl %es, %eax\n\t"
        "nop\n\t"
        "movl %ss, %eax\n\t"
        "nop");

    return 0;
}