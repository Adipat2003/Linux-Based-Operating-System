# Linux-Based_Operating-System

This repository contains the code for implementing an operating system with specific functionalities such as dynamic memory allocation, file system utilities, and system calls.

## Features

### Dynamic Memory Allocation
The project includes a dynamic memory allocator (similar to malloc). This is achieved by keeping track of where free pages are using a method, then creating a malloc system call that adds a new page to the program's page table or page directory.

### File System Utilities
The open system call provides access to the file system. The call finds the directory entry corresponding to the named file, allocates an unused file descriptor, and sets up any data necessary to handle the given type of file (directory, RTC device, or regular file). The getargs call reads the program's command line arguments into a user-level buffer.

### System Calls
The project supports system calls via a common IDT entry, including syscall value checking, register save and restore, and a jump table to C functions that implement the system calls themselves.

## Memory Map
The kernel and video memory are at the same location in virtual memory as they are in physical memory. The kernel code is already loaded at 4 MB, so you need only map virtual memory 4-8 MB to physical memory at 4-8 MB.

## Exception and Interrupt Handling
The project handles exceptions and interrupts. The exception handler uses the printing support to report errors when an exception occurs in the kernel, and squashes any user-level program that produces an exception.

## Extra Credit
Extra credit is available for implementing user-level signals and dynamic memory allocation.

## Troubleshooting
Troubleshooting tips include debugging with QEMU and resolving compile errors related to image corruption.
