#ifndef MORE_TERMINALS_H
#define MORE_TERMINALS_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"
#include "rtc.h"
#include "filesystem.h"
#include "paging.h"
#include "terminal.h"
#include "sys_calls.h"
#include "schedule.h"



typedef struct terminal_info_t
{
	unsigned char buffer[128];
	int buffer_index;
	unsigned char whole_screen_buffer[2000];
	int whole_screen_buffer_index;
	volatile int enter_flag;
	int first_time;
} terminal_info_t;

terminal_info_t terminals[3];

terminal_info_t *curr_term; 
terminal_info_t *prev_term;

terminal_info_t *terminal1;
terminal_info_t *terminal2;
terminal_info_t *terminal3;


void init_terminals();

void before_swap(int next_terminal_number);

void swap_terminal(terminal_info_t **curr_term, terminal_info_t *next_term);

#endif



