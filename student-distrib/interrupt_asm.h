#ifndef INTERRUPT_ASM_H
#define INTERRUPT_ASM_H

void system_call_handler();

void keyboard_int();

void rtc_int();

void pit_int();

void mouse_int();

#endif

