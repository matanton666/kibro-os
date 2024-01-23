.section .text
.global load_gdt
.global load_idt

load_gdt:
	mov 4(%esp), %eax // get gdt pointer argument
	lgdt (%eax)

	// set segments
	mov $0x10, %eax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs
	mov %eax, %ss
	jmp $0x8, $.long_jump // set code segment to 0x8 and go to it
.long_jump:
	ret


load_idt:
	mov 4(%esp), %eax // get idt pointer argument
	lidt (%eax)
	ret