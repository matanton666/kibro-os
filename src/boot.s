/* TODO: get framebuffer info from EBX*/

/* constants for the multiboot header. */
.set MAGIC,    0xE85250D6       /* 'magic number' lets bootloader find the header */
.set ARCHITECTURE, 	  0			/* architecture is protected mode i386 */
.set HEADER_LENGTH,		header_end - header_start
.set CHECKSUM, -(MAGIC + ARCHITECTURE + HEADER_LENGTH) /* checksum of above, to prove we are multiboot */
/* framebuffer tag */
.set TYPE, 5 /* framebuffer tag is of type 5*/
.set FLAG,  0 /* flag to tell if framebuffer does not exist then still boot */
.set SIZE, 20
.set WIDTH, 1024
.set HEIGHT, 768
.set DEPTH, 32

.section .multiboot
header_start:

.align 8
.long MAGIC
.long ARCHITECTURE
.long HEADER_LENGTH
.long CHECKSUM

.align 8
.short TYPE
.short FLAG
.long SIZE
.long WIDTH
.long HEIGHT
.long DEPTH


/* required end tag */
.align 8
.short 0		/* type */
.short 0		/* flags */
.long 8		/* size */

header_end:


.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:


.section .text
.global _start
.type _start, @function
_start:

	mov $stack_top, %esp

	call kernel_main

	cli
	loop1:	
		hlt
		jmp loop1

.size _start, . - _start
