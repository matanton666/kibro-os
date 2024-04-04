
.set EIP_OFFSET_FROM_ESP, 28
.set PCB1_OFFSET_FROM_ESP, EIP_OFFSET_FROM_ESP+4 // 32
.set PCB2_OFFSET_FROM_ESP, EIP_OFFSET_FROM_ESP+8 // 36

// pcb strucure: 
// 0: eip
// 4: esp
// 8: cr3

// offsets of fileds in the PCB struct
.set EIP_OFFSET_IN_PCB, 0 
.set ESP_OFFSET_IN_PCB, 4
.set CR3_OFFSET_IN_PCB, 8

.text
.global switch_task


// function saves cpu state of current task in the stack and in the given pcb
// then swaps stack pointers of current and next task and calls next tasks ip
switch_task:
    // (%esp) = return address
	// 4(%esp) = current pcb
    // 8(%esp) = next pcb


    // save cpu state
    // EIP is already saved on stack
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    pushl %ebp

    // stack after pushes
    // regs: 28 bytes (index. 0)
    // eip: 4 bytes (index. 28)
    // arg1: 4 bytes (index. 32)
    // arg2: 4 bytes (index. 36)

    movl PCB1_OFFSET_FROM_ESP(%esp), %edi // param 1
    movl PCB2_OFFSET_FROM_ESP(%esp), %esi // param 2

    // save special regs on current pcb
    movl EIP_OFFSET_FROM_ESP(%esp), %eax // fetch eip
    movl %eax, (%edi) // save return address (eip) 
    movl %cr3, %eax // fetch cr3
    movl %eax, CR3_OFFSET_IN_PCB(%edi) // save cr3 of current task
    movl %esp, ESP_OFFSET_IN_PCB(%edi) // save esp of current task


    // swap stack pointers
    movl ESP_OFFSET_IN_PCB(%esi), %esp


    // restore cpu state
    movl CR3_OFFSET_IN_PCB(%esi), %eax
    movl %eax, %cr3
    // no need to restore eip because it is already on stack

    popl %ebp
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax

    sti
    ret // jump to EIP meaning finishing the task switch




