.section .text
.global enable_paging
.global switch_pd

enable_paging:
	mov 4(%esp), %eax // get page directory address
	mov %eax, %cr3

	// set paging bit in cr0 (enable paging)
	mov %cr0, %eax 
	or $0x80000001, %eax
	mov %eax, %cr0
	ret

switch_pd: // switch page directory
	mov 4(%esp), %eax
	mov %eax, %cr3
	ret
