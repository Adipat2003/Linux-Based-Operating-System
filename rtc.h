#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "sys_call.h"

#define STATUS_REG_A    0x8A
#define STATUS_REG_B    0x8B
#define STATUS_REG_C    0x8C
#define RTC_REG_PORT    0x70
#define RTC_RW_PORT     0x71
#define RTC_IRQ         0x08


extern void rtc_init();
extern void rtc_handler();

extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t rtc_close(int32_t fd);

#endif
