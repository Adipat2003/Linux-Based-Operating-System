#ifndef _ASM_WRAPPER_H
#define _ASM_WRAPPER_H

#include "scheduler.h"
#include "keyboard.h"
#include "rtc.h"
#include "idt_exceptions.h"
#include "sys_call.h"
#include "x86_desc.h"

#define USER_CS     0x0023
#define USER_DS     0x002B

extern void setup_IRET(uint32_t, uint32_t);
extern void pit_wrapper();
extern void keyboard_wrapper();
extern void rtc_wrapper();
extern void sys_call_link();
extern void jump_to_execute(uint8_t, uint32_t, uint32_t);
extern void scheduler_asm(int32_t, int32_t);

#endif

