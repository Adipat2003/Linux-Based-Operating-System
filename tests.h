#ifndef TESTS_H
#define TESTS_H

extern unsigned int fileSystemStartAddress;

#define PASS 1
#define FAIL 0

#define CAT 0
#define COUNTER 1
#define FISH 2
#define FRAMEZERO 3
#define FRAMEONE 4
#define GREP 5
#define HELLO 6
#define LS 7
#define PINGPONG 8
#define SHELL 9
#define SIGTEST 10
#define SYSERR 11
#define TESTPRINT 12
#define VERYLONG 13
#define WRONGFILENAME 14

#define BUF_SIZE 7000

#define MAX_BYTES_IN_BLOCK  4096

// test launcher
void launch_tests();

extern int deref_test();
extern int divide_zero_test();
extern inline int NMI_test();
extern inline int Overflow_test();
extern inline int Bound_Range_test();
extern inline int Invalid_Opcode_test();
extern inline int sys_call_test();
extern int keyboard_test();
extern int rtc_test();
extern int rtc_kb_test();
extern int paging_test();
extern int kernel_out_of_bound_top();
extern int kernel_out_of_bound_bottom();
extern int video_out_of_bound_top();
extern int video_out_of_bound_bottom();

extern int terminal_write_exact();
extern int terminal_write_under();
extern int terminal_write_over();
extern int terminal_read_write_exact();
extern int terminal_read_write_under();
extern int terminal_read_write_over();
// extern int terminal_read_only();
extern int rtc_freq_test();
extern int directory_test();
extern int read_data_test();

#endif /* TESTS_H */
