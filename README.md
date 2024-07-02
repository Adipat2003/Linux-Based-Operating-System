# Linux-Based-Operating-System

This OS is designed with specific functionalities such as dynamic memory allocation, file system utilities, and system calls.

## Dynamic Memory Allocation
- Implements a dynamic memory allocator (similar to malloc).
- Keeps track of free pages.
- Includes a malloc system call that adds a new page to the program's page table or page directory.
- Remember that implementing dynamic memory will not get you any extra credit if you don't have a way of demoing the functionality. For example, writing a short user program or some kind of kernel-level functionality.

## File System Utilities
- The open system call provides access to the file system.
- Finds the directory entry corresponding to the named file.
- Allocates an unused file descriptor.
- Sets up any data necessary to handle the given type of file (directory, RTC device, or regular file).
- If the named file does not exist or no descriptors are free, the call returns -1.

## System Calls
- Supports system calls via a common IDT entry.
- Includes syscall value checking, register save and restore, and a jump table to C functions that implement the system calls themselves.
- The execute call returns -1 if the command cannot be executed, for example, if the program does not exist or the filename specified is not an executable, 256 if the program dies by an exception, or a value in the range 0 to 255 if the program executes a halt system call, in which case the value returned is that given by the program's call to halt.

## Memory Map
- The kernel and video memory are at the same location in virtual memory as they are in physical memory.
- The kernel code is already loaded at 4 MB, so you need only map virtual memory 4-8 MB to physical memory at 4-8 MB.

## Exception and Interrupt Handling
- Handles exceptions and interrupts.
- The exception handler uses the printing support to report errors when an exception occurs in the kernel, and should squash any user-level program that produces an exception, returning control to the shell (the shell should not cause an exception in a working OS).
