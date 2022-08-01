nop:
	as --32 test1.s -o test1.o
	as --32 test2.s -o test2.o
	ld -m elf_i386 -T test.lds test1.o test2.o -o test
	objdump -d -m i8086 test > test_objdump
	ld -m elf_i386 -T test.lds -Map test_map test1.o test2.o -o test
