.text

.globl loadPageDirectory
.align 4
loadPageDirectory:
    push %ebp
    mov %esp, %ebp

    mov 8(%ebp), %eax
    mov %eax, %cr3

    mov %cr4, %eax
    or $0x00000010, %eax
    mov %eax, %cr4

    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    leave
    ret


.globl flush_tlb
.align 4
flush_tlb:
    push %ebp
    mov %esp, %ebp

    push %eax
    pushl %ebx
    pushl %edi
    pushl %esi

    movl %cr3, %eax
    movl %eax, %cr3

    popl %esi
    popl %edi
    popl %ebx
    pop %eax

    leave
    ret
