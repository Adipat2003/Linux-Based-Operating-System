#ifndef SYS_CALL_H_
#define SYS_CALL_H_

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "rtc.h"
#include "terminal.h"
#include "filesys_driver.h"
#include "paging.h"
#include "asm_wrappers.h"

#define MAX_FILENAME_LENGTH                 32
#define PERIOD                              0x7F
#define E                                   0x45
#define L                                   0x4C
#define F                                   0x46
#define SPACE                               0x20
#define BYTES_IN_BLOCK                      4096
#define MB128                               0x8000000
#define VIRTUAL_START_ADDRESS               0x08048000
#define MB4                                 0x00400000
#define RTC_FLAGS                           0x00000001
#define DIRECTORY_FLAGS                     0x00000101
#define FILE_FLAGS                          0x00000201
#define SIXTYNINE                           69
#define SCREEN_MEM                          0x8400000
#define FAIL                                -1

typedef int32_t (*ReadFuncs)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*WriteFuncs)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*OpenFuncs)(const uint8_t* filename);
typedef int32_t (*CloseFuncs)(int32_t fd);

typedef struct{
    ReadFuncs fread;
    WriteFuncs fwrite;
    OpenFuncs fopen;
    CloseFuncs fclose;
}fops_t;

// fops_t fops_ptr_table[3];
  
typedef struct{
    fops_t* fileTablePointer;
    uint32_t inode;
    uint32_t filePos;
    uint32_t flags; //4B: unused, unused, file type, active
}fileDescContents_t;

typedef struct{
    int32_t saved_esp;
    int32_t saved_eip;
    int32_t tss_esp0;
    int32_t tss_ss0;
    int32_t saved_ebp;
    int32_t temp_esp;
    int32_t temp_eip;
    int32_t exec_esp;
    int32_t exec_ebp;
}saves;

typedef struct{
    uint32_t process_id;
    uint32_t parent_id;
    fileDescContents_t file_descriptors[8];
    saves current;
    uint8_t argument[MAX_FILENAME_LENGTH];
}PCB_t;

extern int PIDArray[6];
extern int isShell[6];
int fileType;
extern uint32_t currentPID;
extern uint32_t parentPID;

extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes); 
extern int32_t open (const uint8_t* filename);
extern int32_t close(int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern PCB_t* get_PCB();
extern int32_t PIDs_left();

#endif 
