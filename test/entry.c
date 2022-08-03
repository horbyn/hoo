int entry() {
    __asm__("movl %cs, %eax\n\t"
            "nop\n\t"
            "nop\n\t"
            "movl %ds, %eax");
    
    char *entry = "entry";
    int e;
    return 0;
}