#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "terminal.h"
#include "more_terminals.h"


//1

#define DIRECTORY_SIZE   1024
#define ALIGNMENT        4096
#define ONEKB            1024
#define NOT_PRESENT      0x2
#define FOURMB_PAGE      0x80
#define PRESENT_WRITE_ON 0x3
#define USER_SUPERVISOR  0x4
#define VIDEO_MEMORY     0xB8000
#define VIDEO_INDEX      0xB8
#define FOURKB           0x1000
#define KERNEL           0x400000
#define EXEC             0x8000000
#define USER_PAGE        EXEC / KERNEL
#define EIGHTMB          0x800000
#define FOURMB           0x400000
#define EIGHTKB          0x2000
#define USER_VIDEO       EXEC + FOURMB
#define VIDMAP_INDEX     33   //132 / 4
#define FIRST_ENTRY      0
#define SECOND_ENTRY     1
#define TERM1_PAGE       0xB9000
#define TERM2_PAGE       0xBA000
#define TERM3_PAGE       0xBB000
#define TERM1   	     0xB9
#define TERM2   	     0xBA
#define TERM3    	     0xBB
#define MAX_PROC         6

uint32_t page_directory[DIRECTORY_SIZE] __attribute__((aligned(ALIGNMENT)));
uint32_t first_page_table[DIRECTORY_SIZE] __attribute__((aligned(ALIGNMENT)));
uint32_t vidmap_table[DIRECTORY_SIZE] __attribute__((aligned(ALIGNMENT)));


//initialize paging
void init_paging();
int32_t syscall_exe_setup_page();
int32_t restore_paging(int process_number);
int32_t setup_vidmap_page();
int32_t schedule_page(int process_num);
int32_t terminal_video_paging(uint32_t screen_start, uint32_t terminal_video);


#endif

