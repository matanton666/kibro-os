.section .text
.global enable_paging

enable_paging:
	mov 4(%esp), %eax // get page directory address
	mov %eax, %cr3

	// set paging bit in cr0 (enable paging)
	mov %cr0, %eax 
	xor $0x80000000, %eax
	mov %eax, %cr0
	ret
	
	