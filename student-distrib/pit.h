#ifndef PIT_H
#define PIT_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"
#include "schedule.h"

#define CHANNEL_0    0x40 //for scheduling
#define CHANNEL_1    0x41 //usually not used
#define CHANNEL_2    0x42 //for PC speakers
#define CMD_MODE_REG 0x43
#define CMD_BYTE     0x36

#define SHIFT_EIGHT    8
#define LOW_BYTE_MASK  0xFF
#define TWENTY_MILLISEC 0.02
#define DIVISOR_BASE   1193180
#define IRQ_PIT        0

int32_t pit_open(const uint8_t* filename);
int32_t pit_close(int32_t fd);
int32_t pit_read(int32_t fd, void* buf, int32_t nbytes);
int32_t pit_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t init_pit(int channel);
int32_t pit_interrupt_handler();

int seconds;

#endif

