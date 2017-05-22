#include "paging.h"

//1

/*
*   init_paging()
*   Inputs: NONE
*   Return Value: NONE
*	Function: initilizes a page directory and page table to support paging 
*/
void init_paging()
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	int i; //loop index

	//setting each entry at the page directory to not present 
	for(i = 0; i < ONEKB; i++)
	{
		// This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
		page_directory[i] = NOT_PRESENT;
	}

	// holds the physical address where we want to start mapping these pages to.
	//we will fill all 1024 entries in the table, mapping 4 megabytes
	for(i = 0; i < ONEKB; i++)
	{
    	first_page_table[i] = (i * FOURKB) | NOT_PRESENT;
	}

	uint32_t addr = (uint32_t)first_page_table;
	// Put the newly created page table into our blank page directory. 
	// We do this by setting the first entry in the page directory to the address of our page table.
	// This is for the 0 - 4 MB. 
	page_directory[FIRST_ENTRY] = addr | PRESENT_WRITE_ON; //OSDEV

	//Set the second entry to a single page of 4MB 
	page_directory[SECOND_ENTRY] = KERNEL | FOURMB_PAGE | PRESENT_WRITE_ON; //OSDEV

	//This setting is for video memory to be enabled and present in virtual memory. 
	//Set the video memory entry to present where physical and virtual address are the same, we just need a mapping 
	//NOTE: VIDEO MEMORY is taken from lib.c file!
	first_page_table[VIDEO_INDEX] = VIDEO_MEMORY | PRESENT_WRITE_ON;

	first_page_table[TERM1] = TERM1_PAGE | PRESENT_WRITE_ON; //terminal 1

	first_page_table[TERM2] = TERM2_PAGE | PRESENT_WRITE_ON; //terminal 2

	first_page_table[TERM3] = TERM3_PAGE | PRESENT_WRITE_ON; //terminal 3

    //first: load page directory ////////////OSDEV///////////////////
    //second: enable 4MB paging in CR4 ////////FROM LECTURE /////////
    //three: Enable paging at the end! ////////OSDEV////////////////

	asm volatile ("mov %0, %%eax\n	\
			mov %%eax, %%cr3\n \
			mov %%cr4, %%eax\n \
			or $0x00000010, %%eax\n \
			mov %%eax, %%cr4 \n \
			mov %%cr0, %%eax\n \
			or $0x80000000, %%eax\n	\
			mov %%eax, %%cr0\n \
			"
			:	
			: "r" (page_directory)
			: "%eax");
}


/*
*   syscall_exe_setup_page(int process_number)
*   Inputs: current process number being executed
*   Return Value: 0 on success, -1 on failure
*	Function: Sets up 4MB paging for execution of programs 
*   Maps 128 MB to 8MB // (process_number * FOURMB) + EIGHTMB;
*/
int32_t syscall_exe_setup_page(int process_number)
{
	if(process_number > MAX_PROC || process_number < 0)
		return -1;
	
	uint32_t address = (process_number * FOURMB) + EIGHTMB;
	page_directory[(uint32_t)USER_PAGE] = address | FOURMB_PAGE | PRESENT_WRITE_ON | USER_SUPERVISOR;

	asm volatile ("movl %%cr3, %%edx\n	\
			movl %%edx, %%cr3\n \
			"
			:	
			:
			: "%edx"); //tlb flush - taken from practice final ece391

	return 0;
}

/*
*   restore_paging(int process_number)
*   Inputs: parent process number being executed
*   Return Value: 0 on success, -1 on failure
*	Function: Sets up 4MB paging for execution of parent program 
*   Maps 128 MB to 8MB // (process_number * FOURMB) + EIGHTMB;
*/
int32_t restore_paging(int process_number)
{
	if(process_number > MAX_PROC || process_number < 0)
		return -1;
	
	
	uint32_t address = (process_number * FOURMB) + EIGHTMB;
	page_directory[(uint32_t)USER_PAGE] = address | FOURMB_PAGE | PRESENT_WRITE_ON | USER_SUPERVISOR;

	asm volatile ("movl %%cr3, %%edx\n	\
			movl %%edx, %%cr3\n \
			"
			:	
			:
			: "%edx"); //tlb flushed
	return 0;
}

/*
*   setup_vidmap_page()
*   Inputs: NONE
*   Return Value: 0 on success, -1 on failure
*	Function: Sets up user video memory paging and maps to kernel video mem.
*   Maps kernel and user space video memory 
*/
int32_t setup_vidmap_page()
{
	//basically repeats the paging for setting up a video memory paging
	if((uint32_t)vidmap_table == NULL)
		return -1;

	page_directory[(uint32_t)VIDMAP_INDEX] = (uint32_t)vidmap_table | PRESENT_WRITE_ON | USER_SUPERVISOR;
	vidmap_table[FIRST_ENTRY] = VIDEO_MEMORY | PRESENT_WRITE_ON | USER_SUPERVISOR;

	asm volatile ("movl %%cr3, %%edx\n	\
			movl %%edx, %%cr3\n \
			"
			:	
			:
			: "%edx"); //tlb flushed
	return 0;
}

/*
*   schedule_page(int process_num)
*   Inputs: current process number 
*   Return Value: 0 on success, -1 on failure
*	Function: Restores paging in scheduling for next process to execute
*  
*/
int32_t schedule_page(int process_num)
{
	if(process_num > MAX_PROC || process_num < 0)
		return -1;

	

	uint32_t address = (process_num * FOURMB) + EIGHTMB;
	page_directory[(uint32_t)USER_PAGE] = address | FOURMB_PAGE | PRESENT_WRITE_ON | USER_SUPERVISOR;
	asm volatile ("movl %%cr3, %%edx\n	\
			movl %%edx, %%cr3\n \
			"
			:	
			:
			: "%edx"); //tlb flushed
	return 0;
}

/*
*   terminal_video_paging(uint32_t screen_start, uint32_t terminal_video)
*   Inputs: User screen start pointer, terminal video pointer to map 
*   Return Value: 0 on success, -1 on failure
*	Function: Sets up user video memory paging and maps to terminal video mem.
*   
*/
int32_t terminal_video_paging(uint32_t screen_start, uint32_t terminal_video)
{
	if(screen_start == NULL || terminal_video == NULL)
		return -1;
	
	uint32_t index = screen_start / FOURMB;
	page_directory[index] = (uint32_t)vidmap_table | PRESENT_WRITE_ON | USER_SUPERVISOR;
	vidmap_table[0] = terminal_video | PRESENT_WRITE_ON | USER_SUPERVISOR;
	asm volatile ("movl %%cr3, %%edx\n	\
			movl %%edx, %%cr3\n \
			"
			:	
			:
			: "%edx"); //tlb flushed
	return 0;
}

