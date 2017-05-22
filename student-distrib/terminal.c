#include "terminal.h"
#include "keyboard.h"
#include "sys_calls.h"

/*
* int terminal_open(const uint8_t *filename);
*   Inputs: filename (don't use)
*   Return Value: 0
*	Function: initializes keyboard input and sets up buffers
*/
int terminal_open(const uint8_t *filename){
	//just initilaize the keyboard and clear buffer (set all to 0)
	int i;
	buffer_index = -1;
	whole_screen_buffer_index = 0;
	for(i = 0; i < BUFFER_SIZE; i++){
		buffer[i] = 0;
	}
	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		whole_screen_buffer[whole_screen_buffer_index] = 0;
	}
	enter_flag = 0;
	update_cursor(0);
	keyboard_init();
	return 0;
}

/*
* int terminal_close(int32_t fd);
*   Inputs: fd (don't use)
*   Return Value: 0
*	Function: closes the terminal by returning 0
*/
int terminal_close(int32_t fd){
	//just return 0
	return 0;
}

/*
* int terminal_read(int32_t fd, void *buf, int32_t nbytes);
*   Inputs: fd, buf, nbytes (not used)
*   Return Value: bytes read from buffer
*	Function: return data from one line that has been terminated by pressing enter
*/
int terminal_read(int32_t fd, void *buf, int32_t nbytes){
	
	status_bar();
	int i;
	int retval = -1;
	//printf("termin read");
	while(!enter_flag) {
		cli();
		sti();
	}
		enter_flag = 0;
		prev_command_index = buffer_index;
		//clear *buf before
		for(i = 0; i < BUFFER_SIZE; i++){
			*(uint8_t *)(buf + i) = 0;
		}

		for(i = 0; i <= buffer_index; i++){	//loop until new line character is encountered.  if it is never encountered, return -1
			//put into buf
			*(uint8_t  *)(buf + i) = buffer[i];
			prev_command[i] = buffer[i];
			if(buffer[i] == '\n'){
				retval = i + 1;
				break;
			}
		}
		return retval;
}

/*
* int terminal_write(int32_t fd, const void *buf, int32_t nbytes);
*   Inputs: fd not used nbytes bytes to write, buf is what to print
*   Return Value: bytes written to screen
*	Function: writes a message to the screen
*/
int terminal_write(int32_t fd, const void *buf, int32_t nbytes){
	//write to screen the whole_screen_buffer
	int bytes_written = 0;
	int x,y;
	int8_t* wbuf = (int8_t *)buf;
	cli();						//no interrupts when doing this
	while(bytes_written < nbytes)
	{
		x = whole_screen_buffer_index % TERMINAL_WIDTH;
		y = whole_screen_buffer_index / TERMINAL_WIDTH;
		putc_terminal(*wbuf,x,y);
		whole_screen_buffer[whole_screen_buffer_index] = *wbuf;
		if(*wbuf != '\n' && *wbuf != '\r'){
			whole_screen_buffer_index++;
		}
		if(whole_screen_buffer_index >= WHOLE_SCREEN_BUFFER_SIZE){
    		vertical_scroll();
    	}
    	wbuf++;
    	bytes_written++;
	}
	update_cursor(whole_screen_buffer_index);
	buffer_index = -1;

	return bytes_written;
}



