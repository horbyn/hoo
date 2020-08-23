
system:     file format elf32-i386
system
architecture: i386, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x00000000

Program Header:
    LOAD off    0x00001000 vaddr 0x00000000 paddr 0x00000000 align 2**12
         filesz 0x0000027c memsz 0x0000027c flags rwx
    NOTE off    0x00001260 vaddr 0x00000260 paddr 0x00000260 align 2**2
         filesz 0x0000001c memsz 0x0000001c flags r--
0x6474e553 off    0x00001260 vaddr 0x00000260 paddr 0x00000260 align 2**2
         filesz 0x0000001c memsz 0x0000001c flags r--
   STACK off    0x00000000 vaddr 0x00000000 paddr 0x00000000 align 2**4
         filesz 0x00000000 memsz 0x00000000 flags rw-

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         000001bc  00000000  00000000  00001000  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .text.__x86.get_pc_thunk.ax 00000004  000001bc  000001bc  000011bc  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .got.plt      0000000c  000001c0  000001c0  000011c0  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  3 .eh_frame     00000094  000001cc  000001cc  000011cc  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .note.gnu.property 0000001c  00000260  00000260  00001260  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .comment      00000024  00000000  00000000  0000127c  2**0
                  CONTENTS, READONLY
SYMBOL TABLE:
00000000 l    d  .text	00000000 .text
000001bc l    d  .text.__x86.get_pc_thunk.ax	00000000 .text.__x86.get_pc_thunk.ax
000001c0 l    d  .got.plt	00000000 .got.plt
000001cc l    d  .eh_frame	00000000 .eh_frame
00000260 l    d  .note.gnu.property	00000000 .note.gnu.property
00000000 l    d  .comment	00000000 .comment
00000000 l    df *ABS*	00000000 init.c
00000000 l    df *ABS*	00000000 display.c
0000019b l     F .text	00000021 print_char
00000000 l    df *ABS*	00000000 
000001c0 l     O .got.plt	00000000 _GLOBAL_OFFSET_TABLE_
000001bc g     F .text.__x86.get_pc_thunk.ax	00000000 .hidden __x86.get_pc_thunk.ax
00000000 g     F .text	0000014e kernel_enter
0000014e g     F .text	0000004d clear_screen



Disassembly of section .text:

00000000 <kernel_enter>:
   0:	f3 0f 1e fb          	endbr32 
   4:	55                   	push   %ebp
   5:	89 e5                	mov    %esp,%ebp
   7:	53                   	push   %ebx
   8:	83 ec 14             	sub    $0x14,%esp
   b:	e8 ac 01 00 00       	call   1bc <__x86.get_pc_thunk.ax>
  10:	05 b0 01 00 00       	add    $0x1b0,%eax
  15:	c7 45 f4 00 80 0b 00 	movl   $0xb8000,-0xc(%ebp)
  1c:	89 c3                	mov    %eax,%ebx
  1e:	e8 2b 01 00 00       	call   14e <clear_screen>
  23:	8b 45 f4             	mov    -0xc(%ebp),%eax
  26:	8d 50 01             	lea    0x1(%eax),%edx
  29:	89 55 f4             	mov    %edx,-0xc(%ebp)
  2c:	c6 00 48             	movb   $0x48,(%eax)
  2f:	8b 45 f4             	mov    -0xc(%ebp),%eax
  32:	8d 50 01             	lea    0x1(%eax),%edx
  35:	89 55 f4             	mov    %edx,-0xc(%ebp)
  38:	c6 00 0f             	movb   $0xf,(%eax)
  3b:	8b 45 f4             	mov    -0xc(%ebp),%eax
  3e:	8d 50 01             	lea    0x1(%eax),%edx
  41:	89 55 f4             	mov    %edx,-0xc(%ebp)
  44:	c6 00 65             	movb   $0x65,(%eax)
  47:	8b 45 f4             	mov    -0xc(%ebp),%eax
  4a:	8d 50 01             	lea    0x1(%eax),%edx
  4d:	89 55 f4             	mov    %edx,-0xc(%ebp)
  50:	c6 00 0f             	movb   $0xf,(%eax)
  53:	8b 45 f4             	mov    -0xc(%ebp),%eax
  56:	8d 50 01             	lea    0x1(%eax),%edx
  59:	89 55 f4             	mov    %edx,-0xc(%ebp)
  5c:	c6 00 6c             	movb   $0x6c,(%eax)
  5f:	8b 45 f4             	mov    -0xc(%ebp),%eax
  62:	8d 50 01             	lea    0x1(%eax),%edx
  65:	89 55 f4             	mov    %edx,-0xc(%ebp)
  68:	c6 00 0f             	movb   $0xf,(%eax)
  6b:	8b 45 f4             	mov    -0xc(%ebp),%eax
  6e:	8d 50 01             	lea    0x1(%eax),%edx
  71:	89 55 f4             	mov    %edx,-0xc(%ebp)
  74:	c6 00 6c             	movb   $0x6c,(%eax)
  77:	8b 45 f4             	mov    -0xc(%ebp),%eax
  7a:	8d 50 01             	lea    0x1(%eax),%edx
  7d:	89 55 f4             	mov    %edx,-0xc(%ebp)
  80:	c6 00 0f             	movb   $0xf,(%eax)
  83:	8b 45 f4             	mov    -0xc(%ebp),%eax
  86:	8d 50 01             	lea    0x1(%eax),%edx
  89:	89 55 f4             	mov    %edx,-0xc(%ebp)
  8c:	c6 00 6f             	movb   $0x6f,(%eax)
  8f:	8b 45 f4             	mov    -0xc(%ebp),%eax
  92:	8d 50 01             	lea    0x1(%eax),%edx
  95:	89 55 f4             	mov    %edx,-0xc(%ebp)
  98:	c6 00 0f             	movb   $0xf,(%eax)
  9b:	8b 45 f4             	mov    -0xc(%ebp),%eax
  9e:	8d 50 01             	lea    0x1(%eax),%edx
  a1:	89 55 f4             	mov    %edx,-0xc(%ebp)
  a4:	c6 00 20             	movb   $0x20,(%eax)
  a7:	8b 45 f4             	mov    -0xc(%ebp),%eax
  aa:	8d 50 01             	lea    0x1(%eax),%edx
  ad:	89 55 f4             	mov    %edx,-0xc(%ebp)
  b0:	c6 00 0f             	movb   $0xf,(%eax)
  b3:	8b 45 f4             	mov    -0xc(%ebp),%eax
  b6:	8d 50 01             	lea    0x1(%eax),%edx
  b9:	89 55 f4             	mov    %edx,-0xc(%ebp)
  bc:	c6 00 77             	movb   $0x77,(%eax)
  bf:	8b 45 f4             	mov    -0xc(%ebp),%eax
  c2:	8d 50 01             	lea    0x1(%eax),%edx
  c5:	89 55 f4             	mov    %edx,-0xc(%ebp)
  c8:	c6 00 0f             	movb   $0xf,(%eax)
  cb:	8b 45 f4             	mov    -0xc(%ebp),%eax
  ce:	8d 50 01             	lea    0x1(%eax),%edx
  d1:	89 55 f4             	mov    %edx,-0xc(%ebp)
  d4:	c6 00 6f             	movb   $0x6f,(%eax)
  d7:	8b 45 f4             	mov    -0xc(%ebp),%eax
  da:	8d 50 01             	lea    0x1(%eax),%edx
  dd:	89 55 f4             	mov    %edx,-0xc(%ebp)
  e0:	c6 00 0f             	movb   $0xf,(%eax)
  e3:	8b 45 f4             	mov    -0xc(%ebp),%eax
  e6:	8d 50 01             	lea    0x1(%eax),%edx
  e9:	89 55 f4             	mov    %edx,-0xc(%ebp)
  ec:	c6 00 72             	movb   $0x72,(%eax)
  ef:	8b 45 f4             	mov    -0xc(%ebp),%eax
  f2:	8d 50 01             	lea    0x1(%eax),%edx
  f5:	89 55 f4             	mov    %edx,-0xc(%ebp)
  f8:	c6 00 0f             	movb   $0xf,(%eax)
  fb:	8b 45 f4             	mov    -0xc(%ebp),%eax
  fe:	8d 50 01             	lea    0x1(%eax),%edx
 101:	89 55 f4             	mov    %edx,-0xc(%ebp)
 104:	c6 00 6c             	movb   $0x6c,(%eax)
 107:	8b 45 f4             	mov    -0xc(%ebp),%eax
 10a:	8d 50 01             	lea    0x1(%eax),%edx
 10d:	89 55 f4             	mov    %edx,-0xc(%ebp)
 110:	c6 00 0f             	movb   $0xf,(%eax)
 113:	8b 45 f4             	mov    -0xc(%ebp),%eax
 116:	8d 50 01             	lea    0x1(%eax),%edx
 119:	89 55 f4             	mov    %edx,-0xc(%ebp)
 11c:	c6 00 64             	movb   $0x64,(%eax)
 11f:	8b 45 f4             	mov    -0xc(%ebp),%eax
 122:	8d 50 01             	lea    0x1(%eax),%edx
 125:	89 55 f4             	mov    %edx,-0xc(%ebp)
 128:	c6 00 0f             	movb   $0xf,(%eax)
 12b:	8b 45 f4             	mov    -0xc(%ebp),%eax
 12e:	8d 50 01             	lea    0x1(%eax),%edx
 131:	89 55 f4             	mov    %edx,-0xc(%ebp)
 134:	c6 00 21             	movb   $0x21,(%eax)
 137:	8b 45 f4             	mov    -0xc(%ebp),%eax
 13a:	8d 50 01             	lea    0x1(%eax),%edx
 13d:	89 55 f4             	mov    %edx,-0xc(%ebp)
 140:	c6 00 0f             	movb   $0xf,(%eax)
 143:	b8 00 00 00 00       	mov    $0x0,%eax
 148:	83 c4 14             	add    $0x14,%esp
 14b:	5b                   	pop    %ebx
 14c:	5d                   	pop    %ebp
 14d:	c3                   	ret    

0000014e <clear_screen>:
 14e:	f3 0f 1e fb          	endbr32 
 152:	55                   	push   %ebp
 153:	89 e5                	mov    %esp,%ebp
 155:	83 ec 10             	sub    $0x10,%esp
 158:	e8 5f 00 00 00       	call   1bc <__x86.get_pc_thunk.ax>
 15d:	05 63 00 00 00       	add    $0x63,%eax
 162:	c7 45 f8 00 01 08 00 	movl   $0x80100,-0x8(%ebp)
 169:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%ebp)
 170:	eb 1c                	jmp    18e <clear_screen+0x40>
 172:	8b 45 f8             	mov    -0x8(%ebp),%eax
 175:	8d 50 01             	lea    0x1(%eax),%edx
 178:	89 55 f8             	mov    %edx,-0x8(%ebp)
 17b:	c6 00 20             	movb   $0x20,(%eax)
 17e:	8b 45 f8             	mov    -0x8(%ebp),%eax
 181:	8d 50 01             	lea    0x1(%eax),%edx
 184:	89 55 f8             	mov    %edx,-0x8(%ebp)
 187:	c6 00 0f             	movb   $0xf,(%eax)
 18a:	83 45 fc 01          	addl   $0x1,-0x4(%ebp)
 18e:	81 7d fc cf 07 00 00 	cmpl   $0x7cf,-0x4(%ebp)
 195:	7e db                	jle    172 <clear_screen+0x24>
 197:	90                   	nop
 198:	90                   	nop
 199:	c9                   	leave  
 19a:	c3                   	ret    

0000019b <print_char>:
 19b:	f3 0f 1e fb          	endbr32 
 19f:	55                   	push   %ebp
 1a0:	89 e5                	mov    %esp,%ebp
 1a2:	83 ec 04             	sub    $0x4,%esp
 1a5:	e8 12 00 00 00       	call   1bc <__x86.get_pc_thunk.ax>
 1aa:	05 16 00 00 00       	add    $0x16,%eax
 1af:	8b 45 08             	mov    0x8(%ebp),%eax
 1b2:	88 45 fc             	mov    %al,-0x4(%ebp)
 1b5:	b8 00 00 00 00       	mov    $0x0,%eax
 1ba:	c9                   	leave  
 1bb:	c3                   	ret    

Disassembly of section .text.__x86.get_pc_thunk.ax:

000001bc <__x86.get_pc_thunk.ax>:
 1bc:	8b 04 24             	mov    (%esp),%eax
 1bf:	c3                   	ret    

Disassembly of section .got.plt:

000001c0 <_GLOBAL_OFFSET_TABLE_>:
	...

Disassembly of section .eh_frame:

000001cc <.eh_frame>:
 1cc:	14 00                	adc    $0x0,%al
 1ce:	00 00                	add    %al,(%eax)
 1d0:	00 00                	add    %al,(%eax)
 1d2:	00 00                	add    %al,(%eax)
 1d4:	01 7a 52             	add    %edi,0x52(%edx)
 1d7:	00 01                	add    %al,(%ecx)
 1d9:	7c 08                	jl     1e3 <_GLOBAL_OFFSET_TABLE_+0x23>
 1db:	01 1b                	add    %ebx,(%ebx)
 1dd:	0c 04                	or     $0x4,%al
 1df:	04 88                	add    $0x88,%al
 1e1:	01 00                	add    %eax,(%eax)
 1e3:	00 24 00             	add    %ah,(%eax,%eax,1)
 1e6:	00 00                	add    %al,(%eax)
 1e8:	1c 00                	sbb    $0x0,%al
 1ea:	00 00                	add    %al,(%eax)
 1ec:	14 fe                	adc    $0xfe,%al
 1ee:	ff                   	(bad)  
 1ef:	ff 4e 01             	decl   0x1(%esi)
 1f2:	00 00                	add    %al,(%eax)
 1f4:	00 45 0e             	add    %al,0xe(%ebp)
 1f7:	08 85 02 42 0d 05    	or     %al,0x50d4202(%ebp)
 1fd:	44                   	inc    %esp
 1fe:	83 03 03             	addl   $0x3,(%ebx)
 201:	41                   	inc    %ecx
 202:	01 c3                	add    %eax,%ebx
 204:	41                   	inc    %ecx
 205:	c5 0c 04             	lds    (%esp,%eax,1),%ecx
 208:	04 00                	add    $0x0,%al
 20a:	00 00                	add    %al,(%eax)
 20c:	10 00                	adc    %al,(%eax)
 20e:	00 00                	add    %al,(%eax)
 210:	44                   	inc    %esp
 211:	00 00                	add    %al,(%eax)
 213:	00 a8 ff ff ff 04    	add    %ch,0x4ffffff(%eax)
 219:	00 00                	add    %al,(%eax)
 21b:	00 00                	add    %al,(%eax)
 21d:	00 00                	add    %al,(%eax)
 21f:	00 1c 00             	add    %bl,(%eax,%eax,1)
 222:	00 00                	add    %al,(%eax)
 224:	58                   	pop    %eax
 225:	00 00                	add    %al,(%eax)
 227:	00 26                	add    %ah,(%esi)
 229:	ff                   	(bad)  
 22a:	ff                   	(bad)  
 22b:	ff 4d 00             	decl   0x0(%ebp)
 22e:	00 00                	add    %al,(%eax)
 230:	00 45 0e             	add    %al,0xe(%ebp)
 233:	08 85 02 42 0d 05    	or     %al,0x50d4202(%ebp)
 239:	02 45 c5             	add    -0x3b(%ebp),%al
 23c:	0c 04                	or     $0x4,%al
 23e:	04 00                	add    $0x0,%al
 240:	1c 00                	sbb    $0x0,%al
 242:	00 00                	add    %al,(%eax)
 244:	78 00                	js     246 <_GLOBAL_OFFSET_TABLE_+0x86>
 246:	00 00                	add    %al,(%eax)
 248:	53                   	push   %ebx
 249:	ff                   	(bad)  
 24a:	ff                   	(bad)  
 24b:	ff 21                	jmp    *(%ecx)
 24d:	00 00                	add    %al,(%eax)
 24f:	00 00                	add    %al,(%eax)
 251:	45                   	inc    %ebp
 252:	0e                   	push   %cs
 253:	08 85 02 42 0d 05    	or     %al,0x50d4202(%ebp)
 259:	59                   	pop    %ecx
 25a:	c5 0c 04             	lds    (%esp,%eax,1),%ecx
 25d:	04 00                	add    $0x0,%al
	...

Disassembly of section .note.gnu.property:

00000260 <.note.gnu.property>:
 260:	04 00                	add    $0x0,%al
 262:	00 00                	add    %al,(%eax)
 264:	0c 00                	or     $0x0,%al
 266:	00 00                	add    %al,(%eax)
 268:	05 00 00 00 47       	add    $0x47000000,%eax
 26d:	4e                   	dec    %esi
 26e:	55                   	push   %ebp
 26f:	00 02                	add    %al,(%edx)
 271:	00 00                	add    %al,(%eax)
 273:	c0 04 00 00          	rolb   $0x0,(%eax,%eax,1)
 277:	00 03                	add    %al,(%ebx)
 279:	00 00                	add    %al,(%eax)
	...

Disassembly of section .comment:

00000000 <.comment>:
   0:	47                   	inc    %edi
   1:	43                   	inc    %ebx
   2:	43                   	inc    %ebx
   3:	3a 20                	cmp    (%eax),%ah
   5:	28 55 62             	sub    %dl,0x62(%ebp)
   8:	75 6e                	jne    78 <kernel_enter+0x78>
   a:	74 75                	je     81 <kernel_enter+0x81>
   c:	20 39                	and    %bh,(%ecx)
   e:	2e 33 2e             	xor    %cs:(%esi),%ebp
  11:	30 2d 31 30 75 62    	xor    %ch,0x62753031
  17:	75 6e                	jne    87 <kernel_enter+0x87>
  19:	74 75                	je     90 <kernel_enter+0x90>
  1b:	32 29                	xor    (%ecx),%ch
  1d:	20 39                	and    %bh,(%ecx)
  1f:	2e 33 2e             	xor    %cs:(%esi),%ebp
  22:	30 00                	xor    %al,(%eax)
