#include "schedule.h"

/*
*   sched()
*   Inputs: NONE
*   Return Value: NONE
*	Function: calls the scheduler function with no clobbered registers 
*/
void sched(){
		asm volatile ("call shark_scheduler\n	\
			"
			:	
			: 
			: "%eax", "%ebx", "%ecx", "%edx", "%edi", "%esi"); //no esp, ebp , they SHOULD be clobbered
}

/*
*   shark_scheduler()
*   Inputs: NONE
*   Return Value: NONE
*	Function: restores sp and bp for current process and switches to next process for execution
*/
void shark_scheduler()
{
	cli();
	if(PID[0] == 0){
		return;
	}
	if(running_processes == 1){
		asm("movl %%esp, %0" : "=r" (curr_p->schedule_sp)); //save stack pointer in PCB
		asm("movl %%ebp, %0" : "=r" (curr_p->schedule_bp)); //save base pointer in PCB
		return;	
	}

	int next_num;
	//calculate next process
	//curr_p = pcbs_for_scheduling[current_process_number];
	if(current_process_number + 1 >= 3){
		next_num = 0;
	}
	else{
		next_num = current_process_number + 1;
	}
	next_p = pcbs_for_scheduling[(next_num)]; 
	if(next_p == NULL){
		if(current_process_number + 2 >= 3){
			next_num = 0;
		}
		else{
			next_num = current_process_number + 2;
		}
		next_p = pcbs_for_scheduling[(next_num)];
		if(next_p == NULL){
			next_p = pcbs_for_scheduling[current_process_number];
		} 
	}

/*	if(curr_p == next_p || curr_p == NULL || next_p == NULL){
		asm("movl %%esp, %0" : "=r" (curr_p->schedule_sp)); //save stack pointer in PCB
		asm("movl %%ebp, %0" : "=r" (curr_p->schedule_bp)); //save base pointer in PCB
		return;
	}
*/
	current_process_number = next_p->terminal_number - 1;

	
	uint8_t *next_vidmap;
	vidmap(&next_vidmap);
		
	schedule_page(next_p->process_id);
		

	
	if(next_p->terminal_number != curr_p->terminal_number)
	{
		if(next_p->terminal_number == 1)
			terminal_video_paging((uint32_t)next_vidmap,(uint32_t)0xB9000);
		if(next_p->terminal_number == 2)
			terminal_video_paging((uint32_t)next_vidmap,(uint32_t)0xBA000);
		if(next_p->terminal_number == 3)
			terminal_video_paging((uint32_t)next_vidmap,(uint32_t)0xBB000);
	}

	//tss.ss0 = KERNEL_DS;
	tss.esp0 = EIGHTMB - ((next_p->process_id) * EIGHTKB) - TSS;

	asm("movl %%esp, %0" : "=r" (curr_p->schedule_sp)); //save stack pointer in PCB
	asm("movl %%ebp, %0" : "=r" (curr_p->schedule_bp)); //save base pointer in PCB
	
	//context switch
	
	curr_p = next_p;
	cli();
	asm volatile ("movl %0, %%esp\n	\
			movl %1, %%ebp\n \
			"
			:	
			: "r" (next_p->schedule_sp), "r" (next_p->schedule_bp) //fixed a bug
			 ); //no esp, ebp , they SHOULD be clobbered
	
}
