#ifndef IDT_H
#define IDT_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "mouse.h"
#include "sys_calls.h"
#include "pit.h"

#define SYSCALL_ENTRY 0x80
#define RTC_ENTRY 0x28
#define KEYB_ENTRY 0x21
#define MOUSE_ENTRY 0x2C
#define PIT_ENTRY 0x20
#define BUF_SIZE_EXCEPTION 30

/* expetion handlers for the first 31 entries and system call case */
void exception_0();
void exception_1();
void exception_2();
void exception_3();
void exception_4();
void exception_5();
void exception_6();
void exception_7();
void exception_8();
void exception_9();
void exception_10();
void exception_11();
void exception_12();
void exception_13();
void exception_14();
void exception_15();
void exception_16();
void exception_17();
void exception_18();
void exception_19();
void exception_20();
void exception_21();
void exception_22();
void exception_23();
void exception_24();
void exception_25();
void exception_26();
void exception_27();
void exception_28();
void exception_29();
void exception_30();
void exception_31();
void sys_call_80();
void rtc_40();

//initiliazes our IDT
int init_IDT();


#endif

