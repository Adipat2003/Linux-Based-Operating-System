#include "terminal.h"

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ    0x01
#define KEYBOARD_PORT   0x60
#define SCAN_CODE_SIZE  0x3B
#define SCAN_CODE_MIN   0x00
#define SCAN_CODE_MAX   0x3A

#define LSHIFT_PRESS    0x2A
#define RSHIFT_PRESS    0x36
#define CAPSLOCK_PRESS  0x3A
#define LSHIFT_RELEASE  0xAA
#define RSHIFT_RELEASE  0xB6
#define CTRL_PRESS      0x1D
#define CTRL_RELEASE    0x9D
#define ALT_PRESS       0x38
#define ALT_RELEASE     0xB8
#define ENTER_PRESS     0x1C
#define BSPACE_PRESS    0x0E
#define BSPACE_RELEASE  0x8E
#define F1_PRESS        0x3B
#define F2_PRESS        0x3C
#define F3_PRESS        0x3D

// volatile int read_flag;

extern void kb_init();
extern void kb_handler();

#endif
