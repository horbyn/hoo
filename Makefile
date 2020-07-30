S_SRC = $(shell find . -name "*.s")
S_OBJ = $(patsubst %.s, %.o, $(S_SRC))

AS=as
LD=ld

nop: image bochs
	

image: boot/bootsect
	@if [ ! -f "fd1_44M.img" ]; then \
		dd if=/dev/zero of=hd1_44M.img bs=1474560 count=1; \
	fi

bochs:
	dd if=boot/bootsect of=hd1_44M.img bs=512 count=1 conv=notrunc
	DISPLAY=:0 /usr/bin/bochs

clean:
	rm -f $(S_OBJ) boot/bootsect

boot/bootsect: boot/bootsect.o
	$(LD) --oformat binary -e start -Ttext 0x7c00 -m elf_i386 $< -o $@

$(S_OBJ): %.o: %.s
	$(AS) --32 $< -o $@

