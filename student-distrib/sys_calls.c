#include "sys_calls.h"

filesystem_t *file_system_shark = &file_sys_arr[0]; //so that the whole filesystem could be a pointer

//file operation tables for each device, file, directory

//these could be pointers?
uint32_t file_operation_rtc[FOPS_SIZE] = {(uint32_t)rtc_open,(uint32_t)rtc_close,(uint32_t)rtc_read,(uint32_t)rtc_write};
uint32_t file_operation_files[FOPS_SIZE] = {(uint32_t)filesystem_open,(uint32_t)filesystem_close,(uint32_t)filesystem_read,(uint32_t)filesystem_write};
uint32_t file_operation_directory[FOPS_SIZE] = {0,0,(uint32_t)read_directory,0};
uint32_t file_operation_stdin[FOPS_SIZE] = {0,0,(uint32_t)terminal_read,0};
uint32_t file_operation_stdout[FOPS_SIZE] = {0,0,0, (uint32_t)terminal_write};
process_control_block_t * most_recent_pcb = NULL;


int PID[MAX_PID] = {0}; //process id array so that there could be a maximum number of 6 processes
/*
 * int32_t halt(uint8_t halt)
 *   DESCRIPTION: System call for halt. 
 *	 	Halts the most recent process by closing fd and passing the stack to its parent. 
 *		If the process is the parent of itself, just execute shell again.
 *   INPUTS: status to halt
 *   OUTPUTS: NONE
 *   RETURN VALUE: NEVER returns. Jumps to last part of execute. 
 *   SIDE EFFECTS: The most recent program is halted. 
 */
int32_t halt(uint8_t status)
{
	int i;
	int process_to_halt_index; 						//to extract the pcb to halt 
	int32_t retval;           					//retval to jump to execute 
	uint32_t kb_ptr, ks_ptr;	               //need to restore these based on parent or not
	process_control_block_t *temp_pcb = NULL;
	int curr_term_num; 

	cli();
	//get terminal number
	if(curr_term == terminal1){
		curr_term_num = 1;
	}
	else if(curr_term == terminal2){
		curr_term_num = 2;
	}
	else if(curr_term == terminal3){
		curr_term_num = 3;
	}

	//pcb to halt is the most recent pcb with the same terminal number (from end of PID array)
	for(i = MAX_PID - 1; i >= 0; i--){
		temp_pcb = (process_control_block_t *)(EIGHTMB - (i + 1) * EIGHTKB);
		if(PID[i] == 1 && temp_pcb->terminal_number == curr_term_num) //when equal to curr term, get the process to halt
		{
			process_to_halt_index = i;
			break;
		}
	}

	process_control_block_t *pcb_to_halt = (process_control_block_t *)(EIGHTMB - (process_to_halt_index + 1) * EIGHTKB);;
	PID[process_to_halt_index] = 0;
	pcb_to_halt->process_state = 0;
	running_processes--;
	



	//check pcbs parent.
	if(pcb_to_halt->parent->process_id == pcb_to_halt->process_id){
		//if null, this is the first process (shell), and it cannot be halted
		kb_ptr = pcb_to_halt->kernel_base_ptr; //coudl also be parent
		ks_ptr = pcb_to_halt->kernel_stack_ptr;
		//remove the process from pcbs for scheduling
		pcbs_for_scheduling[pcb_to_halt->terminal_number - 1] = NULL;
		execute((uint8_t *)"shell");
	}
	else{
		//need to restore kernel stack ptr, kernel base pointer and stack pointer of the PARENT
		kb_ptr = pcb_to_halt->parent->kernel_base_ptr;
		ks_ptr = pcb_to_halt->parent->kernel_stack_ptr;
		pcbs_for_scheduling[pcb_to_halt->terminal_number - 1] = pcb_to_halt->parent;
		restore_paging(pcb_to_halt->parent->process_id);
		most_recent_pcb = most_recent_pcb->parent;
	}


	//close relevant file descriptors (PID array) (the last index that is filled)
	//mark the flags members of file array as not in use
	//do we need to call close on these?
	for(i = STD_OUT+1; i < MAX_FILE; i++){
		close(i);
	}

	tss.esp0 = EIGHTMB - ((pcb_to_halt->parent->process_id) * EIGHTKB) - TSS;
	retval = status | 0x00000000;
	//use execute pcb to restore esp, ebp of the child process -- does this work?
	asm volatile ("movl %0, %%eax\n	\
			movl %1, %%esp\n \
			movl %2, %%ebp\n \
			jmp RETURN_FROM_HALT\n \
			"
			:	
			: "r" (retval), "r" (ks_ptr), "r" (kb_ptr) //fixed a bug
			: "%eax"); //no esp, ebp , they SHOULD be clobbered


	//this doesnt happen
	sti();
	return retval;
}

/*
 * int32_t execute(const uint8_t* command)
 *   DESCRIPTION: System call for execute. 
 *	 	Attempts to load and execute a new program, handing off the processor to the new program
 *	 	until it terminates.
 *   INPUTS: pointer to the command string
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: a program is loaded and executed in processor. 
 */
int32_t execute(const uint8_t* command)
{
	status_bar();
	//// 1. PARSE THE COMMAND STRING /////////////////////
	//// 2. CHECK FILE VALIDITY - CHECK IF IT IS EXECUTABLE /////////////////////////
	//// 3. SET UP PAGING //////////////////////////////
	//// 4. LOAD FILE INTO MEMORY //////////////////////////
	//// 5. Create PCB/Open FDs /////////////////////////////
	//// 6. PREPARE FOR CONTEXT SWITCH /////////////////////
	//// 7. PUSH IRET CONTEXT TO STACK ////////////////////
	//local variables and buffers 
	int i,j,k;
	uint8_t cmd_buffer[COMMAND];
	int halt_retval = 0;
	//uint8_t arg_buf[125];
	char fname[MAX_FILE_NAME_SIZE];
	int fname_length;
	int length;
	int executable;
	dentry_t hold[1];
	dentry_t *temp = &hold[0];
	int inodenum,size;
	inode_t *node = file_system_shark->fs_inodes;
	uint8_t buffer_execute[file_system_shark->fs_boot_block->num_data_blocks * FOUR_KILOBYTES];
	uint8_t entry_buffer[ELF_SIZE];
	uint32_t entry_pt;
	uint32_t base_pointer;
	uint32_t stack_pointer;
	int process_i;
	int process_num;
	int available_process_flag;
	int child_flag = 0;
	process_control_block_t *temp_pcb = NULL;
	int parent_idx = -1;
	int32_t cflag = 0;

	length = strlen((int8_t*)command);

	if(length == 0) //error check
		return -1;
	if(command == NULL) //just making sure 
		return -1;

	//init local variables
	base_pointer = 0;
	stack_pointer = 0;
	executable = 0;
	fname_length = 0;
	available_process_flag = 0;
	child_flag = 0;

	//two buffers, one for filename to read from filesystem, other for getargs, cmd buffer

	for(k = 0; k < COMMAND; k++) //clear the buffers from garbage values
	{
		cmd_buffer[k] = '\0';
	}

	for(k = 0; k < MAX_FILE_NAME_SIZE; k++)
	{
		fname[k] = '\0';
	}
	//// 1. PARSE THE COMMAND STRING /////////////////////

	//first word is the file name of the program to be executed. 

	for(i = 0, j = 0; command[i] != '\0' && command[i] != '\n' ; i++) //get the file name
	{
		if(command[i] != ' '){
			fname[j] = command[i]; //will break when hits the first space
			j++;
			cflag = 1;
		}
		if(cflag == 1 && command[i] == ' '){
			break;
		}
	}

	fname_length = i;

	for(j = fname_length; command[j] != '\0'; j++) 
	{
		cmd_buffer[j - fname_length] = command[j+1];
	}

	i = 0;
	j = 0;
	//for checkpoint 4, strip off leading spaces for getargs - parse args 
	while(cmd_buffer[j] != '\0')
	{
		cmd_buffer[i] = cmd_buffer[j++];
		if(cmd_buffer[i] != ' ')
			i++;
	}
  	cmd_buffer[i] = '\0';

	//// 2. CHECK FILE VALIDITY - CHECK IF IT IS EXECUTABLE /////////////////////////

  	//read dentry by name fills up the info based on fname like inode index and length
	if(read_dentry_by_name((uint8_t*)fname,temp) == -1) //check file validity by name 
		return -1;

	inodenum = temp->inode_index; //get inode index
	size = node[inodenum].length; //get length of file 

	//read data into buffer to check for stufff
	read_data(inodenum,0,buffer_execute,size); //read data

	if((buffer_execute[0] == ELF0) && (buffer_execute[1] == 'E') && (buffer_execute[2] == 'L') && (buffer_execute[3] == 'F')) //ELF: executable linked format 
	{
		executable = 1;
	}

	if(executable == 0) //failure if not executable 
		return -1;

	//Get process number from process ID array 
	for(process_i = 0; process_i < MAX_PID; process_i++)
	{
		if(!PID[process_i])
		{
			process_num = process_i;
			PID[process_i] = 1;
			available_process_flag = 1;
			if(process_i > 0)
			{
				child_flag = 1;
			}
			break;
		}
	}
	
	if(available_process_flag == 0) //if there are no processes available return -1
	{
		terminal_write(1,"Maximum number of processes reached!\n",MAX_MSG);
		return 0;
	}
	running_processes++;

	//Get entry point to the program - 4byte unsigned integer in bytes 24-27 of the executable 
	read_data(inodenum,ENTRY_PT,entry_buffer,ELF_SIZE);
	////this would get the 4 bytes from buffer? stackoverflow? 
	entry_pt = entry_buffer[0] | ((uint32_t)entry_buffer[1] << SHIFT_1) | (uint32_t)(entry_buffer[2] << SHIFT_2) | ((uint32_t)entry_buffer[3] << SHIFT_3);  

	//// 3. SET UP PAGING //////////////////////////////

	//Appendix C: Memory Map and Task Specification

	//From Doc:
	//Set up a single 4 MB page diretory entry that maps virtual address 0x08000000 (128 MB) to the right
	//physical memory address (either 8 MB or 12 MB).

	
	if(syscall_exe_setup_page(process_num) == -1)
		return -1;

	//// 4. LOAD FILE INTO MEMORY //////////////////////////

	//Appendix C: Memory Map and Task Specification

	file_loader(inodenum,0,(uint8_t*)LOADER,size); //load the file into memory address from DOC

	//// 5. Create PCB/Open FDs /////////////////////////////


	//finish FD?
	process_control_block_t * execute_pcb = (process_control_block_t *)(EIGHTMB - ((process_num + 1) * EIGHTKB));
	if(process_num == 0){
		execute_pcb->terminal_number = 1;
	}
	//most_recent_pcb = execute_pcb;
	//init stdin stdout 
	execute_pcb->file_array[0].file_operation = file_operation_stdin;
	execute_pcb->file_array[1].file_operation = file_operation_stdout;

	if((fname[0] == 's') && (fname[1] == 'h') && (fname[2] == 'e') && (fname[3] == 'l') && (fname[4] == 'l')) // 5
		execute_pcb->shell_flag = 1;

	execute_pcb->process_id = process_num; //no scheduling for this checkpoint? 

	execute_pcb->process_state = 1; //1 for running process 


	for(i = 0; i < STD_OUT+1; i++)
		execute_pcb->file_array[i].flags = 1;

	for(i = STD_OUT+1; i < MAX_FILE; i++)
		execute_pcb->file_array[i].flags = 0;

	//put the argumnet in pcb
	strcpy((int8_t*)execute_pcb->argument,(int8_t*)cmd_buffer);



	////// 6. PREPARE FOR CONTEXT SWITCH /////////////////////


	//set pcb's terminal
	cli();
	if(prev_term == terminal1 || prev_term == NULL){
		execute_pcb->terminal_number = 1;
	} 
	else if(prev_term == terminal2){
		execute_pcb->terminal_number = 2;
	}
	else if(prev_term == terminal3){
		execute_pcb->terminal_number = 3;
	}
	
	//current_process_number = execute_pcb->terminal_number - 1;
 	if(running_processes == 1){
		current_process_number = 0; 
	}
	curr_p = execute_pcb;
	current_process_number = execute_pcb->terminal_number - 1; 



	pcbs_for_scheduling[execute_pcb->terminal_number - 1] = execute_pcb;

	if(child_flag == 1)
	{
		if(most_recent_pcb->process_state == 1){
			//need to asssign the parent to the same terminal # that called it.
			if(curr_term->first_time == 0){
				//first time running shell in the new terminal, so it has no parent - set parent to itself
				execute_pcb->parent = (process_control_block_t *)(EIGHTMB - ((process_num + 1) * EIGHTKB)); 
				execute_pcb->first_shell = 1;
				curr_term->first_time = 1;
			}
			else{
				execute_pcb->first_shell = 0;
				//else we need to assign the parent to the current terminal process, so need to check which fd corresponds to the current terminal that called this.
				//the parent will be the first pcb from the end of PID array that is the correct terminal number.
				for(i = MAX_PID - 1; i >= 0; i--){
					temp_pcb = (process_control_block_t *)(EIGHTMB - (i + 1) * EIGHTKB);
					if((temp_pcb->terminal_number == execute_pcb->terminal_number) && (i != process_num) && PID[i] == 1){
						parent_idx = i;
						break;
					}
				}				
				if(parent_idx == -1){
					execute_pcb->parent = (process_control_block_t *)(EIGHTMB - ((process_num + 1) * EIGHTKB)); 
				}
				else{
					execute_pcb->parent = (process_control_block_t *)(EIGHTMB - (parent_idx + 1) * EIGHTKB);
				}
			}
		}
	}
	else
	{
		execute_pcb->parent = (process_control_block_t *)(EIGHTMB - ((process_num + 1) * EIGHTKB)); //if no child, set parent to itself
	}

	most_recent_pcb = execute_pcb;

	//following two lines are from stackoverflow...
	asm("movl %%esp, %0" : "=r" (stack_pointer)); //save stack pointer in PCB
	asm("movl %%ebp, %0" : "=r" (base_pointer)); //save base pointer in PCB


	execute_pcb->parent->kernel_stack_ptr = stack_pointer;
	execute_pcb->parent->kernel_base_ptr = base_pointer;

	
	// Write tss.esp0/ebp0 with the new process kernel stack? 
	tss.ss0 = KERNEL_DS;
	tss.esp0 = EIGHTMB - ((process_num) * EIGHTKB) - TSS; //Piazza


	///// 7. PUSH IRET CONTEXT TO STACK ////////////////////

	context_switching(entry_pt); //go to the user space, privilege level 3

	asm volatile ("RETURN_FROM_HALT: \n	\
			movl %%eax, %0\n \
			"
			: "=r" (halt_retval)
			:
			: "%eax");

	return halt_retval;
}

/*
 * int32_t read(int32_t fd, void* buf, int32_t nbytes)
 *   DESCRIPTION: System call for read. 
 *	 	Checks for valid fd and then calls the corresponding 
 *	 	until it terminates.
 *   INPUTS: file descriptor, buffer to read, nbytes to read
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: The read is called depending on fops table. 
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{ 
	if(fd < 0 || fd > FD_MAX || fd == 1)
		return -1;

	if(most_recent_pcb->file_array[fd].flags == 0){
		return -1;
	}

	return 	(*(read_call)(most_recent_pcb->file_array[fd].file_operation[2]))(fd, buf, nbytes);
	
}

/*
 * int32_t write(int32_t fd, void* buf, int32_t nbytes)
 *   DESCRIPTION: System call for write. 
 *	 	Calls the corresponding write function based on file descriptor. 
 *   INPUTS: file descriptor, buffer to read, nbytes to read
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: The write is called depending on fops table. 
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	if(fd < STD_OUT || fd > FD_MAX)
		return -1;

	if(most_recent_pcb->file_array[fd].flags == 0){
		return -1;
	}

	return (*(write_call)(most_recent_pcb->file_array[fd].file_operation[3]))(fd, buf, nbytes);

}

/*
 * int32_t open(const uint8_t* filename)
 *   DESCRIPTION: System call for open. 
 *	 	Finds an unused file descriptor, sets it flags to 1 and returns that fd.
 *   INPUTS: filename to open in shark filesystem
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: The read is called depending on fops table. 
 */
int32_t open (const uint8_t* filename)
{
	process_control_block_t * open_pcb = most_recent_pcb;
	dentry_t hold[1];
	dentry_t *file_dentry = &hold[0];
	int check = read_dentry_by_name(filename,file_dentry);
	int i;
		if(check<0)
		 return -1;

		for (i = STD_OUT+1; i < MAX_FILE; i++)//Iterate after stdin and stdout
		{
			if(open_pcb->file_array[i].flags == 0) //If file is not being used ...
			{
				if (file_dentry->file_type == 0)
				{
					open_pcb->file_array[i].file_operation = file_operation_rtc;
					open_pcb->file_array[i].inode_index = file_dentry->inode_index;
					open_pcb->file_array[i].file_position = 0;
					open_pcb->file_array[i].flags = 1;
				}

				if(file_dentry->file_type == 1)
				{
					open_pcb->file_array[i].file_operation = file_operation_directory;
					open_pcb->file_array[i].inode_index = file_dentry->inode_index;
					open_pcb->file_array[i].file_position = 0;
					open_pcb->file_array[i].flags = 1;
				}

				if(file_dentry->file_type == 2)
				{
					open_pcb->file_array[i].file_operation = file_operation_files;
					open_pcb->file_array[i].inode_index = file_dentry->inode_index;
					open_pcb->file_array[i].file_position = 0;
					open_pcb->file_array[i].flags = 1;
				}

				return i;
			}
	}

	return -1;
}

/*
 * int32_t close(int32_t fd)
 *   DESCRIPTION: System call for close. 
 *	 	Closes the files for the file descriptor passed in. Sets the flags to 0.
 *   INPUTS: file descriptor to close
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: NONE
 */
int32_t close (int32_t fd)
{
	process_control_block_t * open_pcb = most_recent_pcb;

	if(fd < STD_OUT+1 || fd > FD_MAX || open_pcb == NULL || open_pcb->file_array[fd].flags != 1){
		return -1;
	}

	open_pcb->file_array[fd].flags = 0;
	open_pcb->file_array[fd].file_operation = NULL;

	return 0;
}

/*
 * int32_t getargs(uint8_t* buf, int32_t nbytes)
 *   DESCRIPTION: System call for getargs. 
 *	 	Gets the argument buffer from the current process and writes it into buffer. 
 *   INPUTS: file descriptor, buffer to read, nbytes to read
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: NONE 
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
	process_control_block_t * arg_pcb = most_recent_pcb;

	if(buf == NULL)
		return -1;
	if(nbytes == 0)
		return -1;
	if(arg_pcb->argument[0] == 0){
		return -1;
	}

	strcpy((int8_t*)buf, (int8_t*)arg_pcb->argument);
	return 0;
}

/*
 * int32_t vidmap(uint8_t** screen_start)
 *   DESCRIPTION: System call for vidmap. 
 *	 	Maps kernel video memory to user video memory at a pre-set virtual address. 132 MB
 *   INPUTS: start of screen
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: NONE 
 */
int32_t vidmap (uint8_t** screen_start)
{
	//screen_start is a pointer that we need to fill up with the video memory address
	
	//error check - *screen_start must be within 128 and 132 MB
	if(screen_start == NULL)
	{
		return -1;
	}

	if(screen_start == (uint8_t**)FOURMB) //for syserr...
	{
		return -1;
	}

	if(screen_start < (uint8_t**)EXEC || screen_start > (uint8_t**)USER_VIDEO)
		return -1;
	
	//set up a page, and give the pointer access to this page
	if(setup_vidmap_page() == -1)
		return -1;

	*screen_start = (uint8_t *)USER_VIDEO;
	return USER_VIDEO;	//return 
}

/*
 * int32_t set_handler (int32_t signum, void* handler_address)
 *   DESCRIPTION: EXTRA CREDIT - NOT IMPLEMENTED
 *   INPUTS: signal number, handler address
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: NONE 
 */
int32_t set_handler (int32_t signum, void* handler_address)
{
	//extra credit
	return -1;
}

/*
 * int32_t sigreturn(void)
 *   DESCRIPTION: EXTRA CREDIT - NOT IMPLEMENTED
 *   INPUTS: void
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: NONE 
 */
int32_t sigreturn(void)
{
	//extra credit
	return -1;
}

/*
 * int32_t context_swotching(uint32_t entry_point)
 *   DESCRIPTION: context switching - goes into user space after execute
 *   INPUTS: entry_point
 *   OUTPUTS: NONE
 *   RETURN VALUE: -1 if cannot be executed. 0 on success.
 *   SIDE EFFECTS: User space stack is created after iret and program jumps to there. 
 */
void context_switching(uint32_t entry_point)
{
	//0x2B = USER_DS
	//0x83FFFFC = user space stack pointer
	//0x200 = for setting interrupt flag
	//0x23 = USER_CS
	
	cli();
	asm volatile (" mov $0x2B, %%eax\n\t"
			"mov %%ax, %%ds\n\t"
			"pushl $0x2B\n"
			"movl $0x83FFFFC, %%eax\n"
			"pushl %%eax\n"
			"pushfl\n"
			"popl %%eax\n"
			"orl $0x200, %%eax\n"
			"pushl %%eax\n"
			"pushl $0x23\n"
			"pushl %0\n"
			"iret\n"
			:	
			: "r" (entry_point)
			: "%eax");
	
}
