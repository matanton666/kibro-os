.section .text
.global enable_paging

enable_paging:
	mov 4(%esp), %eax
	mov %eax, %cr3
	
	mov %cr0, %eax
	xor $0x80000000, %eax
	mov %eax, %cr0
	ret
	
	