/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL             0
#define fourMB           0x400000
#define eightMB          0x800000
#define fourKB           4096
#define eightKB          8192
#define alligner         4
#define vidmap_start     0x08000000
#define vidmap_end       0x08400000
#define VIDEO            0xB8000

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

//struct holding information about each directory entry 
//This will be 64 bytes
typedef struct{
    int8_t fileName[32]; //holding 32 bytes of name for the file name (32 characters are possible)
    uint32_t fileType; //If 0 then RTC, if 1 then directory, if 2 then regular file
    uint32_t inodeNum; 
    uint8_t reserved[24];//24 Bytes are then reserved
}dentry_t;

//struct that holds information about the boot block
//The first 4 will hold 64 bytes, the last will hold 4,032 bytes, resulting in 4096 bytes
typedef struct{
    uint32_t numOfDirectoryEntries;
    uint32_t numOfInodes;
    uint32_t numOfDataBlocks;
    uint8_t reserved[52];       //52 Bytes reserved
    dentry_t dirEntries[63];    //You can have 63 possible directory entries, each holding 63 bytes of information
}bootBlockT;

typedef struct{
    uint32_t length;
    uint32_t numOfDataBlocks[1023]; //4092 bytes
}inode_t;

/* defining the struct for 4 MB directory entries */
typedef struct __attribute__ ((packed, aligned(alligner))) PDE_4MB_t{
        uint32_t present            : 1;
        uint32_t read_write         : 1;
        uint32_t user_supervisor    : 1;
        uint32_t write_through      : 1;
        uint32_t cache_disable      : 1;
        uint32_t accessed           : 1;
        uint32_t dirty              : 1;
        uint32_t page_size          : 1;
        uint32_t global             : 1;
        uint32_t available          : 3;
        uint32_t pat                : 1;
        uint32_t space              : 9;
        uint32_t addr_31_22         : 10;
} PDE_4MB_t;

/* defining the struct for 4 KB directory entries */
typedef struct __attribute__ ((packed, aligned(alligner))) PDE_4KB_t {
        uint32_t present            : 1;
        uint32_t read_write         : 1;
        uint32_t user_supervisor    : 1;
        uint32_t write_through      : 1;
        uint32_t cache_disable      : 1;
        uint32_t accessed           : 1;
        uint32_t available_2        : 1;
        uint32_t page_size          : 1;
        uint32_t available          : 4;
        uint32_t addr_31_12         : 20;
} PDE_4KB_t;

/* defining the struct for 4 KB table entries */
typedef struct __attribute__ ((packed, aligned(alligner))) PTE_4KB_t {
        uint32_t present            : 1;
        uint32_t read_write         : 1;
        uint32_t user_supervisor    : 1;
        uint32_t write_through      : 1;
        uint32_t cache_disable      : 1;
        uint32_t accessed           : 1;
        uint32_t dirty              : 1;
        uint32_t pat                : 1;
        uint32_t global             : 1;
        uint32_t available          : 3;
        uint32_t addr_31_12         : 20;
} PTE_4KB_t;

/* unuioning for the */
typedef union PDE_t {
        struct PDE_4KB_t KB_four;
        struct PDE_4MB_t MB_four;
} PDE_t;

#endif /* ASM */

#endif /* _TYPES_H */
