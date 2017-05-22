#ifndef SCHEDULE_H
#define SCHEDULE_H


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
#include "more_terminals.h"
#include "pit.h"
#include "sys_calls.h"

volatile int current_process_number;

void shark_scheduler();
void sched();
#endif

