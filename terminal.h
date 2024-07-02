#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "lib.h"
#include "sys_call.h"

#define KB_BUF_SIZE     128

typedef struct terminal_t{
    int32_t kb_buf[KB_BUF_SIZE];
    int32_t kb_buf_chars;
    int32_t read_flag;
    int32_t x_screen;
    int32_t y_screen;
    int32_t x_cursor;
    int32_t y_cursor;
    int32_t pid_idx; //Will be either 0-5
    int32_t pids[4];
    int32_t video_mem;
    uint32_t rtc_flag;
    uint32_t rtc_counter;
    uint32_t rtc_counter_reset;
}terminal_t;

terminal_t terminal;
terminal_t terminals[3];
extern int32_t active_terminal;
extern int32_t previous_terminal;
extern int32_t pidFlag;
extern int32_t initializeFlag;
extern int32_t status2;
extern int32_t status3;

extern int32_t terminial_init();
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t bad_open(const uint8_t* filename);
extern int32_t bad_close(int32_t fd);
extern int32_t bad_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t bad_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t switch_terminal (int32_t next_terminal);
#endif
