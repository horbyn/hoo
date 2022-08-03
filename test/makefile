nop:
	gcc -c -m32 entry.c -o entry.o
	gcc -c -m32 test1.c -o test1.o
	gcc -c -m32 test2.c -o test2.o
	as  --32 boot.s  -o boot.o
	ld  -m elf_i386 boot.o -o boot.elf
	objcopy -S -O binary boot.elf  boot
	ld  -m elf_i386 -T test.lds -Map test.map entry.o test1.o test2.o -o kernel
