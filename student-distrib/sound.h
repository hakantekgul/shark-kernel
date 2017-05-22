#ifndef SOUND_H
#define SOUND_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"
#include "schedule.h"
#include "sys_calls.h"

#define DSP_RESET  0x6
#define DSP_READ   0xA
#define DSP_WRITE  0xC
#define DSP_RD_BUF 0xE
#define DSP_INT    0xF
#define BASE       0x220
#define BIT_SEVEN  0x80
#define RESET_CHECK 0xAA
#define SAMPLING   0x41
#define SHIFTING   8
#define MASK_EIGHT 0xFF

void init_sound();
void reset_sb();
void write_sb(uint8_t w);
uint8_t read_sb();
void set_rate(uint16_t freq);
int init_dma_transfer();

#endif

