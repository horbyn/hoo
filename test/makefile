nop:
	gcc -c -m32 entry.c -o entry.o
	gcc -c -m32 test1.c -o test1.o
	gcc -c -m32 test2.c -o test2.o
	as  --32 boot.s  -o boot.o
	ld  -m elf_i386 -T test.lds -Map test.map entry.o test1.o test2.o -o kernel.elf
	objcopy -S -O binary kernel.elf kernel
	ld  --oformat binary -Ttext 0x7c00 -m elf_i386 boot.o -o boot
	dd if=/dev/zero of=test.img bs=1474560 count=1
	dd if=boot of=test.img bs=512 count=1 conv=notrunc
	dd if=kernel of=test.img bs=512 count=3 seek=1 conv=notrunc
