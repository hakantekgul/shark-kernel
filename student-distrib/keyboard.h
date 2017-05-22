#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "kboard_map.h"
#include "terminal.h"
#include "interrupt_asm.h"
#include "more_terminals.h"
#include "sys_calls.h"

#define KEYBOARD_IO_PORT 0x64
#define DATA_PORT 0x60

#define ENTER_KEY 28
#define BACKSPACE_KEY 14
#define CAPS_LOCK_KEY 58
#define SHIFT1_KEY 42
#define SHIFT2_KEY 54
#define RELEASE_SHIFT1_KEY 170
#define RELEASE_SHIFT2_KEY 182
#define RELEASE_SHIFT3_KEY 89
#define RELEASE_SHIFT4_KEY 240
#define CTRL_KEY 29
#define RELEASE_CTRL_KEY 157
#define L_KEY 38
#define CTRL1_KEY 2
#define CTRL2_KEY 3
#define CTRL3_KEY 4
#define CTRL4_KEY 5
#define CTRL5_KEY 6
#define ALT_KEY 56
#define RELEASE_ALT_KEY 184
#define UP_KEY 72
#define DOWN_KEY 80
#define F1_KEY 59
#define F2_KEY 60
#define F3_KEY 61


#define LETTER_INDEX1 16
#define LETTER_INDEX2 25
#define LETTER_INDEX3 30
#define LETTER_INDEX4 38
#define LETTER_INDEX5 44
#define LETTER_INDEX6 50



/*initialize keyboard */
void keyboard_init(void);

void keyboard_handler1(void);

/* keyboard interrupt handler */
void keyboard_handler(void);

#endif

