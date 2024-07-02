.data
    syscall_max     = 10        # the maximum allowable syscall number (inclusive)
    syscall_min     = 0         # the minimum allowable syscall number (non-inclusive)
    USER_CS         = 0x0023    # user code segment
    USER_DS         = 0x002B    # user data segment
    KERNEL_CS       = 0x0010    # kernal code segment
    KERNEL_DS       = 0x0018    # kernel data segment
    STACK_START     = 0x8400000 # start of user memory
    ARG_1           = 8
    ARG_1_ALT       = 4
    ARG_2           = 12
    ARG_2_ALT       = 8
    ARG_3           = 16
    TABLE_SCALAR    = 4

.text

# PIT Wrapper
# 
# Pushes registers and calls handler
# Inputs: None
# Outputs: None
# Side Effects: Saves register values before interrupt is handled
#
.globl pit_wrapper
.align 4

pit_wrapper:
    pushal  # push all registers
    pushfl  # push flags
    call pit_handler # call pit handler
    popfl   # pop flags
    popal   # pop all registers
    iret


# Keyboard Wrapper
# 
# Pushes registers and calls handler
# Inputs: None
# Outputs: None
# Side Effects: Saves register values before interrupt is handled
#
.globl keyboard_wrapper
.align 4

keyboard_wrapper:
    pushal  # push all registers
    pushfl  # push flags
    call kb_handler # call keyboard handler
    popfl   # pop flags
    popal   # pop all registers
    iret


# RTC Wrapper
# 
# Pushes registers and calls handler
# Inputs: None
# Outputs: None
# Side Effects: Saves register values before interrupt is handled
#
.global rtc_wrapper
.align 4

rtc_wrapper:
    pushal  # push all registers
    pushfl  # push flags
    call rtc_handler    # call rtc handler
    popfl   # pop flags
    popal   # pop all registers
    iret


# System Call Linkage
# 
# Pushes registers and calls handler
# Inputs: None
# Outputs: None
# Side Effects: None
#
.global sys_call_link
.align 4

sys_call_link:
    # except eax, push all registers and flags
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %edi
    pushl %esi    
    pushfl

    # call System_Call

    cmpl $syscall_min, %eax # check if sys call is less than 1
    jle sys_call_done_bad 
    cmpl $syscall_max, %eax # check if sys call is greater than 10
    jg sys_call_done_bad
    
    # zero index eax
    addl $-1, %eax

    # edx holds arg 3, ecx holds arg 2, ebx holds arg 1
    pushl %edx
    pushl %ecx
    pushl %ebx
    call *sys_call_table(, %eax, TABLE_SCALAR) # call correct sys call based on arg
    popl %ebx
    popl %ecx
    popl %edx
    
    sys_call_done:
        # all registers should be popped and return
        popfl
        popl %esi
        popl %edi
        popl %edx
        popl %ecx
        popl %ebx
        iret
    
    sys_call_done_bad:
        # move error val to eax, pop all registers, and return
        movl $-1, %eax
        popfl
        popl %esi
        popl %edi
        popl %edx
        popl %ecx
        popl %ebx
        iret

# jump table for system calls
sys_call_table:
    .long halt, execute, read, write, open, close, getargs, vidmap


# Setup IRET
# 
# Called in Execute, sets up stack to go into user space
# Inputs: EIP - instruction pointer (starting address of user program, bytes 24-27)
#         ESP - stack pointer
# Outputs: None
# Side Effects: Going to user space
#
.globl setup_IRET
.align 4

setup_IRET:

    movl ARG_1_ALT(%esp), %ecx  # holds EIP
    movl ARG_2_ALT(%esp), %edx  # holds ESP
    
    # clearing registers
    xorl %ebx, %ebx
    xorl %eax, %eax

    # USER DS, ESP, EFLAGS, CS, EIP
    movw $USER_DS, %bx
    pushl %ebx  # push new data segment
    xorl %ebx, %ebx

    pushl %edx  # push ESP

    pushfl  # temporarily push flags

    popl %eax
    orl $0x200, %eax    # enable interrupt flag
    pushl %eax  # push flags again
    
    movw $USER_CS, %bx    
    pushl %ebx  # push new code segment 

    pushl %ecx  # push EIP
    iret


# Jump To Execute
# 
# Called in Halt, sets up stack to change to parent process
# Inputs: Status - process termination status
#         Saved ESP - ESP of parent process
#         Saved EBP = EBP of parent process
# Outputs: None
# Side Effects: Goes back to the parent program
#
# jumpToExecute(status, parent_pcb->saved_esp, parent_pcb->saved_ebp)
.globl jump_to_execute
.align 4

jump_to_execute:
    # setup stack
    pushl %ebp
    movl %esp, %ebp

    # clearing registers
    xorl %esi, %esi
    xorl %eax, %eax
    
    movb ARG_1(%ebp), %bl
    movb %bl, %al # Holds the return value

    movl ARG_2(%ebp), %esi # Holds ESP
    movl %esi, %esp

    movl ARG_3(%ebp), %esi # Holds EBP
    movl %esi, %ebp

    leave
    ret


.globl scheduler_asm
.align 4

scheduler_asm:
    # setup stack
    pushl %ebp
    movl %esp, %ebp

    xorl %esi, %esi

    movl ARG_1(%ebp), %esi
    movl %esi, %esp

    movl ARG_2(%ebp), %esi
    movl %esi, %ebp

    leave
    ret
