#ifndef RTC_H
#define RTC_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"

#define RTC_PORT_1 0x70 //port 1 of RTC
#define NMI_DISABLE 0x80 //to disable NMI --- check OSDEV
#define REGISTER_B 0x8B 
#define REGISTER_A 0x8A
#define CMOS 0x71
#define NMI_ENABLE 0x7F
#define IRQ_RTC    8
#define REGISTER_C 0x0C
#define BOTTOM4    0xF0
#define INIT_FREQ  15
#define BIT_SIX    0x40
#define MAX_FREQ   1024

/* initializes Real Time Clock */
void init_rtc();

/* interrupt handler for RTC */ 
void handle_rtc_interrupt();

int rtc_set_freq(int rate);
int32_t rtc_open(const uint8_t* filename);
int rtc_close(int32_t fd);
int rtc_read(int32_t fd, void* buf, int32_t nbytes);
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int log_base_2(int number);
void rtc_test();
void rtc_read_test();



#endif

