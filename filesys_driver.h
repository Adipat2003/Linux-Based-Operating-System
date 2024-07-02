#ifndef FILESYS_DRIVER_H_
#define FILESYS_DRIVER_H_

#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "sys_call.h"

//Initialize the file system
extern void filesys_init();

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

//File functions
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);

//Directory functions
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_open(const uint8_t* filename);
int32_t directory_close(int32_t fd);

//A variable that is going to be used as the starting address for the file system (always changes)
uint32_t totalInodes;
uint32_t totalDataBlocks;
bootBlockT* blockZero;
uint32_t inodePointer;
uint32_t dataBlockPointer;
int dirIndex;
extern unsigned int fileSystemStartAddress;
extern dentry_t dentry_global;

#endif
