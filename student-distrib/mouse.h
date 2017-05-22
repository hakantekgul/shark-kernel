#ifndef MOUSE_H
#define MOUSE_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"
#include "rtc.h"
#include "terminal.h"
#include "sys_calls.h"
#include "more_terminals.h"

#define MOUSE_PORT1 0x64
#define ENABLE_BIT  0xA8
#define MOUSE_PORT2 0x60
#define COMPAQ      0x20
#define DEFAULT     0xF6
#define ENABLE2     0xF4
#define MOUSE_IRQ   12
#define SENDING     0xD4
#define ATTRIB_MOUSE 0x3
#define CURSOR      219


void init_mouse();
void mouse_wait(unsigned char type);
void mouse_handler();
int32_t mouse_open(const uint8_t* filename);
int mouse_close(int32_t fd);
void mouse_write(unsigned char a_write, int32_t fd, const void* buf, int32_t nbytes);
unsigned char mouse_read(int32_t fd, void* buf, int32_t nbytes);
void init_cursor();
void move_mouse(int x, int y);

#endif

