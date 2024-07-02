#ifndef _IDT_HANDLERS_H
#define _IDT_HANDLERS_H
#include "idt_exceptions.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "rtc.h"
#include "asm_wrappers.h"

#define SYS_CALL      0x80
#define PIT_INTR      0x20
#define KEYBOARD_INTR 0x21
#define RTC_INTR      0x28
#define EXCEPTION_COUNT 20


extern void init_IDT();

#endif
