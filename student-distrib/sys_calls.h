#ifndef SYS_CALLS_H
#define SYS_CALLS_H

#include "debug.h"
#include "i8259.h"
#include "lib.h"
#include "multiboot.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "idt.h"
#include "rtc.h"
#include "mouse.h"
#include "filesystem.h"
#include "paging.h"
#include "terminal.h"
#include "more_terminals.h"

#define FOPS_SIZE  4
#define MAX_PID    6
#define STD_OUT    1
#define STD_IN     0
#define MAX_FILE   8
#define COMMAND    125
#define ELF_SIZE   4
#define ELF0       0x7F
#define ENTRY_PT   24
#define SHIFT_1    8
#define SHIFT_2    16
#define SHIFT_3    24
#define LOADER     0x08048000
#define TSS        4
#define FD_MAX     7
#define MAX_MSG    40

int PID[MAX_PID];

int32_t halt(uint8_t status);

int32_t execute(const uint8_t* command);

void context_switching(uint32_t entry_point);

int32_t read (int32_t fd, void* buf, int32_t nbytes);

int32_t write (int32_t fd, const void* buf, int32_t nbytes);

int32_t open (const uint8_t* filename);

int32_t close (int32_t fd);

int32_t getargs (uint8_t* buf, int32_t nbytes);

int32_t vidmap (uint8_t** screen_start);

int32_t set_handler (int32_t signum, void* handler_address);


typedef int32_t (*write_call)(int32_t fd, const void* buf, int32_t nbytes);
typedef int32_t (*read_call)(int32_t fd, void* buf, int32_t nbytes);

typedef struct file_descriptor_t
{
	uint32_t *file_operation;
	uint32_t inode_index;
	uint32_t file_position;
	uint32_t flags;
} file_descriptor_t;

// PCB Explanation:
// This struct holds the info on the process that is being executed, halted, or running
// With this information, you can basically do anything you want in the kernel/user space
// It is critical that this information is implemented correctly or it may break the whole kernel after context switches
typedef struct process_control_block_t
{
	file_descriptor_t file_array[8]; 				//an array of file descriptors where max is 8 files
	struct process_control_block_t *parent;			//parent of the pcb
	uint32_t process_id;							//process number of the current pcb
	uint8_t  argument[125];           				//argument that is passed in from command
	uint32_t process_state;							//running process = 1
	uint32_t signal;								//extra credit
	uint32_t kernel_stack_ptr; 						//kernel stack pointer of process 
	uint32_t kernel_base_ptr;						//kernel base pointer of process
	uint32_t terminal_number; 						//the temrinal number process is running on
	uint32_t first_shell;							//whether the process is the first shell in a terminal
	uint32_t shell_flag;							//checks whether the process is a shell
	uint32_t schedule_sp;							//stack ptr for scheduling
	uint32_t schedule_bp;							//base ptr for scheduling
} process_control_block_t;


//fops tables
uint32_t file_operation_rtc[4];
uint32_t file_operation_files[4];
uint32_t file_operation_directory[4];
uint32_t file_operation_stdin[4];
uint32_t file_operation_stdout[4];


process_control_block_t * pcbs_for_scheduling[3]; //array of processes in different terminals for scheduling

process_control_block_t * most_recent_pcb; //most recent pcb in the system

process_control_block_t *pcb_scheduling;
	process_control_block_t *curr_p;
	process_control_block_t *next_p;

volatile int running_processes;


#endif

