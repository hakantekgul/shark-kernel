#ifndef TERMINAL_H
#define TERMINAL_H

#include "keyboard.h"
#include "lib.h"

#define BUFFER_SIZE 128
#define WHOLE_SCREEN_BUFFER_SIZE 80 * 24
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define MINUTE_SIXTY    60
#define TEN_TIME        10
#define MINUTE_TIME     5
#define FIFTY_HZ		50


unsigned char buffer[BUFFER_SIZE];
int prev_command_index;
unsigned char prev_command[BUFFER_SIZE];
int buffer_index;
unsigned char whole_screen_buffer[WHOLE_SCREEN_BUFFER_SIZE];
int whole_screen_buffer_index;
volatile int enter_flag;

//open the terminal
extern int terminal_open(const uint8_t *filename);

//close the terminal
extern int terminal_close(int32_t fd);

//read from terminal
extern int terminal_read(int32_t fd, void *buf, int32_t nbytes);

//write to terminal
extern int terminal_write(int32_t fd, const void *buf, int32_t nbytes);



#endif



