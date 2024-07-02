#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "rtc.h"
#include "terminal.h"
#include "filesys_driver.h"
#include "paging.h"
#include "asm_wrappers.h"

#define CMD_REG     0x43
#define CH0         0x40

extern int curr_ter;
extern int status_ter;
//extern int tempPID;
extern int flag0;
extern int flag1;
extern int flag2;
extern int flag3;
// extern int bug_flag;

extern void pit_init();
extern void pit_handler();
void scheduler();

#endif


