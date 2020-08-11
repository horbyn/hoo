S_SRC   = $(shell find . -name "*.s")
S_OBJ_A = $(patsubst %.s, %.o, $(S_SRC))
S_OBJ_F = $(subst ./boot/bootsect.o, , $(S_OBJ_A))

AS=as
LD=ld

nop: image bochs
	

image: 
	@if [ ! -f "fd1_44M.img" ]; then \
		dd if=/dev/zero of=fd1_44M.img bs=1474560 count=1; \
	fi

bochs: boot/bootsect kernel
	dd if=boot/bootsect of=fd1_44M.img bs=512 count=1 conv=notrunc
	dd if=kernel of=fd1_44M.img bs=512 count=2 seek=1 conv=notrunc
	DISPLAY=:0 /usr/bin/bochs

clean:
	rm -f $(S_OBJ_A) boot/bootsect fd1_44M.img system kernel

boot/bootsect: boot/bootsect.o
	$(LD) --oformat binary -e start -Ttext 0x7c00 -m elf_i386 $< -o $@

system: $(S_OBJ_F)
	$(LD) -T kernel.lds -m elf_i386 $< -o $@

kernel: system
	objcopy -I elf32-i386 -S -R ".eh_frame" -R ".comment" -O binary system $@

$(S_OBJ_A): %.o: %.s
	$(AS) --32 $< -o $@

