#include "more_terminals.h"

#define NO_MORE_PROCESSES_SIZE 60
#define PROMPT_SIZE 7
#define MAX_PROCESS_NUM 6
#define KEYBOARD_IRQ 1


unsigned char no_more_processes[] = "Cannot run any more processes, type exit to quit a process.\n";


/*
 * void init_terminals()
 *   DESCRIPTION: initializes terminals for multiple terminal use. 
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Allows multiple terminals 
 */
void init_terminals(){

	current_process_number = -1;

	pcbs_for_scheduling[0] = NULL;
	pcbs_for_scheduling[1] = NULL;
	pcbs_for_scheduling[2] = NULL;

	running_processes = 0;
	int i;
	terminal1 = &terminals[0];
	terminal2 = &terminals[1];
	terminal3 = &terminals[2];
	terminal1->buffer_index = -1;
	terminal1->whole_screen_buffer_index = 0;
	terminal2->buffer_index = -1;
	terminal2->whole_screen_buffer_index = 0;
	terminal3->buffer_index = -1;
	terminal3->whole_screen_buffer_index = 0;
	for(i = 0; i < BUFFER_SIZE; i++){
		terminal1->buffer[i] = 0;
		terminal2->buffer[i] = 0;
		terminal2->buffer[i] = 0;
	}
	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		terminal1->whole_screen_buffer[whole_screen_buffer_index] = 0;
		terminal2->whole_screen_buffer[whole_screen_buffer_index] = 0;
		terminal3->whole_screen_buffer[whole_screen_buffer_index] = 0;
	}
	terminal1->enter_flag = 0;
	terminal2->enter_flag = 0;
	terminal3->enter_flag = 0;

	//first_time tells if we need to execute shell
	terminal1->first_time = 1;
	terminal2->first_time = 0;
	terminal3->first_time = 0;

	//start on terminal1
	curr_term = terminal1;
	prev_term = NULL;
	update_cursor(0);
	keyboard_init();
	return;
}

/*
 * void before_swap(int next_terminal_number)
 *   DESCRIPTION: helper function for swapping terminals. 
 *   INPUTS: next_terminal_number - the terminal to swap to
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: calls swap terminal 
 */
void before_swap(int next_terminal_number){
	
	terminal_info_t *temp_term = NULL;

	if(next_terminal_number == 1){
		temp_term = terminal1;
	}
	if(next_terminal_number == 2){
		temp_term = terminal2;
	}
	if(next_terminal_number == 3){
		temp_term = terminal3;
	}

	status_bar();
	if(running_processes >= MAX_PROCESS_NUM && temp_term->first_time == 0){
		terminal_write(1, no_more_processes, NO_MORE_PROCESSES_SIZE);
		terminal_write(1, "391OS> ", PROMPT_SIZE);
		return;
	}
	if(next_terminal_number == 1){
		if(curr_term != terminal1){
			swap_terminal(&curr_term, terminal1);
		}
	}
	else if(next_terminal_number == 2){
		if(curr_term != terminal2){
			swap_terminal(&curr_term, terminal2);
		}
	}
	else{
		if(curr_term != terminal3){
			swap_terminal(&curr_term, terminal3);
		}
	}
	return;
}

/*
 * void swap_terminal(terminal_info_t **curr_term, terminal_info_t *next_term)
 *   DESCRIPTION: function that swaps terminals
 *   INPUTS: **curr_term - terminal that we are currently in, *next_term - terminal to swap to
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: swaps terminal, and starts shell if necessary
 */
void swap_terminal(terminal_info_t **curr_term, terminal_info_t *next_term){
	int i,x,y;
	unsigned char c;
	status_bar();
	//need to copy the next terminal to video memory (or change register)
	//unsigned char ATTRIB = 0x7;
	uint8_t *v_mem = (uint8_t *)0xB8000;


	prev_term = next_term;
	(*curr_term)->buffer_index = buffer_index;
	(*curr_term)->whole_screen_buffer_index = whole_screen_buffer_index;
	send_eoi(1);
	status_bar();

	unsigned int num;
	if(next_term == terminal1 || next_term == NULL){ //<- wont happen
		num = 1;
	}
	else if(next_term == terminal2){
		num = 2;
	}
	else if(next_term == terminal3){
		num = 3;
	}

	uint8_t *addr = (uint8_t *)term_num_to_address(num);

	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		y = i / TERMINAL_WIDTH;
		x = i % TERMINAL_WIDTH;
		c = *(uint8_t *)(addr + ((TERMINAL_WIDTH * y + x) << 1));
		*(uint8_t *)(v_mem + ((TERMINAL_WIDTH*y + x) << 1)) = c;
        *(uint8_t *)(v_mem + ((TERMINAL_WIDTH*(i / TERMINAL_WIDTH) + (i % TERMINAL_WIDTH)) << 1) + 1) = 0x3 + num;
	}

	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		y = i / TERMINAL_WIDTH;
		x = i % TERMINAL_WIDTH;
		whole_screen_buffer[i] = *(uint8_t *)(addr + ((TERMINAL_WIDTH * y + x) << 1));
	}

	for(i = 0; i < BUFFER_SIZE; i++){
		(*curr_term)->buffer[i] = buffer[i];
	}

	for(i = 0; i < BUFFER_SIZE; i++)
	{
		buffer[i] = next_term->buffer[i];
	}

	buffer_index = next_term->buffer_index;
	whole_screen_buffer_index = next_term->whole_screen_buffer_index;
	update_cursor(whole_screen_buffer_index);
	status_bar();

	*curr_term = next_term;

	if(next_term->first_time == 0){
		next_term->first_time = 1;
		send_eoi(KEYBOARD_IRQ);
		send_eoi(MOUSE_IRQ);
		status_bar();
		execute((uint8_t *)"shell");
	}
	return; //should never return
}


