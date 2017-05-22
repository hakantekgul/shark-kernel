#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "lib.h"
#include "sys_calls.h"

#define FOUR_KILOBYTES 4096
#define ONEKILOBYTE         1023
#define BOOT_BLOCK_SIZE 64
#define MAX_FILE_NAME_SIZE 32
#define RESERVED 52
#define RESERVED2 24
#define FOUR 4
#define BUFFER_TWNY 20
#define FILE_NUM    17
#define MAX1        999
#define MAX2        40000
#define NAME_FILE   32
#define BUFFER_FIFTY 50
#define BUFFER_SIZE10 10
#define ASCII0       48
#define ASCII1       49
#define ASCII2       50
#define BY_NAME_NUM  186
#define FD_CHECK_TWO 2
#define FD_CHECK_SIX 6
#define SHELLSIZE    7
#define TESTBUFFER   10



typedef struct boot_block_t
{
	uint32_t num_dentries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t reserved[RESERVED];
} boot_block_t;

typedef struct dentry_t
{
	uint8_t filename[MAX_FILE_NAME_SIZE];	//max filename is 32 bytes
	uint32_t file_type;
	uint32_t inode_index;
	uint8_t reserved[RESERVED2];
} dentry_t;

typedef struct inode_t
{
	uint32_t length;
	uint32_t data_blocks[(FOUR_KILOBYTES / FOUR) - 1];				//max data block size = 
} inode_t;

typedef struct data_block_t
{
	uint8_t data[FOUR_KILOBYTES];	
} data_block_t;

typedef struct filesystem_t
{
	boot_block_t *fs_boot_block;
	dentry_t *fs_dentries;
	inode_t *fs_inodes;
	data_block_t *fs_data_blocks;
} filesystem_t;


typedef struct write_data_t
{
	uint8_t new_file_name[MAX_FILE_NAME_SIZE];
	uint32_t dentries_filled[63];
	uint32_t inodes_filled[64];
	uint32_t data_blocks_filled[1023];
} write_data_t;

//so  our fs can be a pointer
filesystem_t file_sys_arr[1];
filesystem_t *shark_fs;
write_data_t writing_data[1];
write_data_t *shark_write_data;



void init_filesystem(unsigned int boot_addr);

uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
uint32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

//global variables to hold shark fs data
uint32_t num_dentries_gl; 
uint32_t num_inodes_gl;
uint32_t num_data_blocks_gl;


int32_t filesystem_open(const uint8_t* filename);
int32_t filesystem_close(int32_t fd);
int32_t filesystem_read(int32_t fd, void *buf, int32_t nbytes);
int32_t filesystem_write(int32_t fd, const void *buf, int32_t nbytes);

void read_file_by_name_test();
int read_directory(int32_t fd, void* buf, int32_t nbytes);
void read_by_index_test();
void file_loader(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int check_for_txt(char *buffer);

void init_write_data();
void write_to_file(const void *buf, int nbytes, int new_dentry_idx, int new_inode_idx, int new_data_block_idx);


#endif



