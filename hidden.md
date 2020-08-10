1\  
as --32 boot/bootsect.s -o boot/bootsect.o  
file boot/bootsect.o  
&#8195;&#8195;boot/bootsect.o: ELF 32-bit LSB relocatable, Intel 80386, version 1 (SYSV), not stripped  
ld -e start -Ttext 0x7c00 -m elf_i386 --oformat elf64-x86-64 -o boot/bootsect boot/bootsect.o  
file boot/bootsect  
&#8195;&#8195;boot/bootsect: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, not stripped  
&#8195;&#8195;&#8195;&#8195;===> 输入目标文件ELF32，输出架构i386却指定输出格式elf64-x86-64，则真正输出格式为elf64-x86-64  

2\  
as --64 boot/bootsect.s -o boot/bootsect.o //相比1\，只as --32改为as --64  
file boot/bootsect.o  
&#8195;&#8195;boot/bootsect.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped  
ld -e start -Ttext 0x7c00 -m elf_i386 --oformat elf64-x86-64 -o boot/bootsect boot/bootsect.o  
&#8195;&#8195;ld: i386:x86-64 architecture of input file `boot/bootsect.o' is incompatible with i386 output  
&#8195;&#8195;&#8195;&#8195;===> 输入目标文件ELF64，输出架构i386是不符合的  
&#8195;&#8195;&#8195;&#8195;1\ & 2\ ===> 输入目标文件格式需要与-m 架构格式一致！！  

3\  
as --64 boot/bootsect.s -o boot/bootsect.o //相比2\，把架构elf_i386改为elf_x86_64，再把输出格式改为区别于架构默认的  
file boot/bootsect.o  
&#8195;&#8195;boot/bootsect.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped  
ld -e start -Ttext 0x7c00 -m elf_x86_64 --oformat elf32-i386 -o boot/bootsect boot/bootsect.o  
file boot/bootsect  
&#8195;&#8195;boot/bootsect: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), statically linked, not stripped  
&#8195;&#8195;&#8195;&#8195;===> 输入目标文件ELF64，输出架构x86_64一致，输出目标文件就是所指定的elf32-i386  

4\  
as --64 boot/bootsect.s -o boot/bootsect.o //相比3\，只把输出格式改为32位的另外架构  
file boot/bootsect.o  
&#8195;&#8195;boot/bootsect.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped  
ld -e start -Ttext 0x7c00 -m elf_x86_64 --oformat elf32-x86-64 -o boot/bootsect boot/bootsect.o  
file boot/bootsect  
&#8195;&#8195;boot/bootsect: ELF 32-bit LSB executable, x86-64, version 1 (SYSV), statically linked, not stripped  

5\  
as --64 boot/bootsect.s -o boot/bootsect.o //相比3\，把架构elf_i386改为elf_x86_64，输出格式仍是架构默认的  
file boot/bootsect.o  
&#8195;&#8195;boot/bootsect.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped  
ld -e start -Ttext 0x7c00 -m elf_x86_64 --oformat elf64-x86-64 -o boot/bootsect boot/bootsect.o  
file boot/bootsect  
&#8195;&#8195;boot/bootsect: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, not stripped  
&#8195;&#8195;&#8195;&#8195;3\ & 5\ ===> 输入目标文件ELF64，输出架构x86_64一致，输出目标文件就是--oformat所指定的  