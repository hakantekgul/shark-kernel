#include "filesystem.h"

static volatile int testing_index = 0;
static volatile int dir_index = 0;
volatile int EOF_FLAG = 0;
volatile int SECOND = 0;

/*
* void init_filesystem(unsigned int boot_addr);
*   Inputs:boot_addr -- address of boot block
*   Return Value: none
*	Function: initilaizes our file system
*/
void init_filesystem(unsigned int boot_addr){
	shark_fs = &file_sys_arr[0];
	//cast this boot address as our boot block
	shark_fs->fs_boot_block = (boot_block_t *)boot_addr;
	//start the dentries at address + size of boot block
	shark_fs->fs_dentries = (dentry_t *)(boot_addr + BOOT_BLOCK_SIZE);
	//inodes start at 4KB offset from boot_addr
	shark_fs->fs_inodes = (inode_t *)(boot_addr+ FOUR_KILOBYTES);
	//data blocks start after inodes
	shark_fs->fs_data_blocks = (data_block_t *)(boot_addr + FOUR_KILOBYTES + (shark_fs->fs_boot_block->num_inodes*FOUR_KILOBYTES));

	//initialize struct for writing
	shark_write_data = &writing_data[0];
	init_write_data();
	
	return;
}
/*
* int32_t filesystem_open(const uint8_t *filename)
*   Inputs: filename - name of file we want ot open
*   Return Value: index of dentry with this filename, -1 on failure to open
*	Function: opens a file
*/
int32_t filesystem_open(const uint8_t* filename){

//loops through the filesystem by looking at directory entries and searches for the passed in filename 
//when found, return the index of that dentry 
	//find dentry that is the filename
	int i, check_len=-1;
	uint32_t long_len=MAX_FILE_NAME_SIZE;
	dentry_t *dentry_arr = shark_fs->fs_dentries;
	//loop thru the dentries and compare
	if((strlen((int8_t* )filename)>=1) && (strlen((int8_t* )filename)<=long_len))
		check_len=0;
	for(i = 0; i < shark_fs->fs_boot_block->num_dentries; i++){
		//(strlen((int8_t* )filename)>=strlen((int8_t* )dentry_arr[i].filename))?strlen((int8_t* )dentry_arr[i].filename):strlen((int8_t* )filename);
		if((check_len==0) && (strncmp((int8_t* )filename, (int8_t* )dentry_arr[i].filename,long_len) == 0)){				//if names are the same.
			return i;
		}
	}
	return -1;
}
/*
* int32_t filesystem_close(int32_t fd)
*   Inputs:fd - not used
*   Return Value: 0
*	Function: closes a file
*/
int32_t filesystem_close(int32_t fd){
	return 0;
}
/*
* int32_t filesystem_read(const uint8_t *filename, uint32_t offset, int32_t fd, void *buf, int32_t nbytes)
*   Inputs:filename - name of file, offset - offset into bytes read, fd - not used, buf - to copy into, nbytes - bytes to read
*   Return Value: 0
*	Function: reads a file with filename and puts into buf
*/

int32_t filesystem_read(int32_t fd, void *buf, int32_t nbytes){
	int inode_index = most_recent_pcb->file_array[fd].inode_index;
	if(most_recent_pcb->file_array[fd].file_position >= shark_fs->fs_inodes[inode_index].length){
		return 0;
	}
	int retval = read_data(inode_index, most_recent_pcb->file_array[fd].file_position, buf, nbytes);
	most_recent_pcb->file_array[fd].file_position += retval;
	return retval;
}

/*
* int32_t filesystem_write(int32_t fd, const void *buf, int32_t nbytes);
*   Inputs: fd - not used, buf - buffer to write, nbytes - bytes to write
*   Return Value: bytes written, -1 if cannot write
*	Function: writes to a file
*/
int32_t filesystem_write(int32_t fd, const void *buf, int32_t nbytes){
	
	//first find the available indices for dentry, inode, data block
	if(fd < FD_CHECK_TWO || fd > FD_CHECK_SIX){
		return -1;
	}

	int i;
	if(nbytes > FOUR_KILOBYTES){
		write(1, "This file is too large.\n", RESERVED2);
		write(1, "391OS> ", SHELLSIZE);
		return -1;
	}
	int new_dentry_idx = -1, new_inode_idx= -1, new_data_block_idx= -1;
	for(i = 0; i < BOOT_BLOCK_SIZE-1; i++){
		if(shark_write_data->dentries_filled[i] == 0){
			new_dentry_idx = i;
			break;
		}
	}
	if(new_dentry_idx == -1){
		write(1, "No space for new files.\n", RESERVED2);
		write(1, "391OS> ", SHELLSIZE);
		return -1;
	}

	for(i = 0; i < BOOT_BLOCK_SIZE; i++){
		if(shark_write_data->inodes_filled[i] == 0){
			new_inode_idx = i;
			break;
		}
	}
	if(new_inode_idx == -1){
		write(1, "No space for new files.\n", RESERVED2);
		write(1, "391OS> ", SHELLSIZE);
		return -1;
	}

	for(i = 0; i < ONEKILOBYTE; i++){
		if(shark_write_data->data_blocks_filled[i] == 0){
			new_data_block_idx = i;
			break;
		}
	}
	if(new_data_block_idx == -1){
		write(1, "No space for new files.\n", RESERVED2);
		write(1, "391OS> ", SHELLSIZE);
		return -1;
	}

	if( *(uint8_t *)(buf + nbytes) == 'w'){
		write_to_file(buf, nbytes, new_dentry_idx, new_inode_idx, new_data_block_idx);
		return nbytes;
	}

	for(i = 0; i < MAX_FILE_NAME_SIZE; i++){
		shark_fs->fs_dentries[new_dentry_idx].filename[i] = most_recent_pcb->argument[i];
	}
	//have the relevant indices for entering a file
	//update boot block
	shark_fs->fs_boot_block->num_dentries++;
	shark_fs->fs_boot_block->num_data_blocks++;

	return nbytes;
}


//When sucessful, the first two 
//calls fill in the dentry t block passed as their se ond argument with the file
//name, file type, and inode number for the file, then return 0. The
/*
* uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
*   Inputs:fname - filename dentry - a blank dentry that we fill up
*   Return Value: 0 on success, -1 on failure
*	Function: finds the dentry corresponding to fname, and then fills up dentry with relevant info
*/
uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	

//go through the filesystem and compare the strings with name. When found, fill up the dentry info 
	int i;
	int idx = -1;
	uint32_t long_len=MAX_FILE_NAME_SIZE;
	int check_len = -1; 
	dentry_t *dentry_arr = shark_fs->fs_dentries;
	if((strlen((int8_t* )fname)>=1) && (strlen((int8_t* )fname)<=long_len))
		check_len=0;
	//loop thru the dentries and compare
	for(i = 0; i < shark_fs->fs_boot_block->num_dentries; i++){
		if((check_len==0) && (strncmp((int8_t* )fname, (int8_t* )dentry_arr[i].filename,long_len) == 0)){				//if names are the same.
			idx = i;
			break;				//only 1 with this name.
		}
	}
	if(idx == -1){	//dentry not found
		return -1;
	}
	//now fill in the dentry
	strcpy((int8_t *)dentry->filename, (int8_t *)dentry_arr[idx].filename);
	dentry->file_type = dentry_arr[idx].file_type;
	dentry->inode_index = dentry_arr[idx].inode_index;
	return 0;
}
/*
* uint32_t rread_dentry_by_index(uint32_t index, dentry_t* dentry)
*   Inputs:index - dentry index, dentry - a blank dentry that we fill up
*   Return Value: 0 on success, -1 on failure
*	Function: finds the dentry corresponding to index, and then fills up dentry with relevant info
*/
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	//already have the index. so check validness then do same as read_dentry_by_name
	if(index >= shark_fs->fs_boot_block->num_dentries){		//invalid index
		return -1;
	}
	dentry_t *dentry_arr = shark_fs->fs_dentries;
	//else, copy stuff over
	//now fill in the dentry
	strcpy((int8_t *)dentry->filename, (int8_t *)dentry_arr[index].filename);
	dentry->file_type = dentry_arr[index].file_type;
	dentry->inode_index = dentry_arr[index].inode_index;
	return 0;
}
/*
* uint32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
*   Inputs:inode - the inode to read, offset -byte offset to read, buf - to copy into, length - how many to read
*   Return Value: 0 when read the whole file, length when did not finish
*	Function: reads from the inode
*/
uint32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	//must check that inode is within number of inodes
	if(inode >= shark_fs->fs_boot_block->num_inodes){
		printf("inode too big\n");
		return -1;
	}
	//check if any dentries are bad when doing this
	//read from this inode and put into the buffer. Then return number of bytes put into buffer. If end of file has been reached, return 0
	inode_t the_inode = shark_fs->fs_inodes[inode];
	int i,j;
	char x;
	int length_count = 0;
	uint32_t idx_data_block;
	//offset is the offset into the data_blocks variable of the_inode???
	// i will count bytes read. 
	if(the_inode.length <= offset){		//length of file less than offset
		return 0;
	}
	j = offset % FOUR_KILOBYTES;
	int prev_i = offset / FOUR_KILOBYTES;
	for(i = prev_i; i <= shark_fs->fs_inodes[inode].length / FOUR_KILOBYTES; i++){
		idx_data_block = the_inode.data_blocks[i];
		if(prev_i != i){
			j = 0;
		}
		//j = offset % FOUR_KILOBYTES;
		for(; j < FOUR_KILOBYTES; j++){
			x = shark_fs->fs_data_blocks[idx_data_block].data[j];
			buf[length_count] = x;
			if(length_count + offset == shark_fs->fs_inodes[inode].length){
					return length_count;
			}
			if(length_count >= length-1){	//read length bytes
				return length;
			}
			length_count++;
		}
	}
	return -1; 	//some error 
}
/*
* void read_file_by_name_test()
*   Inputs:none
*   Return Value:none
*	Function: test to read a file by name
*/
void read_file_by_name_test()
{
	clear_screen();
	filesystem_open((uint8_t *)"frame0.txt"); //open file
	dentry_t hold[1];
	dentry_t *temp = &hold[0]; //init pointer
	read_dentry_by_name((uint8_t *)"frame0.txt",temp); //read by name
	int inodenum,j;
	int name_size=0;
	inodenum = temp->inode_index; //get inode index
	
	int size = shark_fs->fs_inodes[inodenum].length;
	uint8_t buffer[MAX2];
	read_data(inodenum,0,buffer,size); //read the contents
	terminal_write(0,buffer,size); //print on terminal
	for(j = 0; j < NAME_FILE; j++) //get name
	{
			buffer[j] = temp->filename[j];
			name_size++;
			if(temp->filename[j] == '\0'){
				break;///
			}
	}
	terminal_write(0,"\n",name_size);
	terminal_write(0,"filename: ",name_size); //print name
	terminal_write(0,buffer,name_size);
}
/*
* void read_directory()
*   Inputs:file descriptor
*   Return Value:length for each directory entry and 0 at the end
*	Function: reads a directory for ls 
*/
int read_directory(int32_t fd, void* buf, int32_t nbytes)
{
	dentry_t hold[1];
	dentry_t *temp = &hold[0]; //init pointer for directory entry
	int i;
	if(read_dentry_by_index(dir_index,temp) != -1)
	{
		for(i = 0; i < NAME_FILE+1; i++)
		{
			*(uint8_t *)(buf + i) = '\0';
		}
		for(i = 0; i < NAME_FILE; i++)
		{
			((int8_t*)(buf))[i] = temp->filename[i];
		}
		dir_index++;
		return (strlen(((int8_t*)(buf)))); //return bytes written
	}
	dir_index = 0;
	return 0;
}
/*
* void file_loader(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
*   Inputs:inode number, offset on file, buffer, length of file to read
*   Return Value:none
*	Function: loads file into memory
*/
void file_loader(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	read_data(inode,offset,buf,length);
	return;
}
/*
* void read_file_by_index_test()
*   Inputs:none
*   Return Value:none
*	Function: testing reading by index
*/
void read_by_index_test()
{
	if(testing_index > shark_fs->fs_boot_block->num_dentries)
		testing_index = 0;
	clear_screen();
	int j,inodenum;
	
	uint8_t buffer[shark_fs->fs_boot_block->num_data_blocks * FOUR_KILOBYTES];
	char buffer_name[32];
	
	dentry_t hold[1];
	dentry_t *temp = &hold[0];
	inode_t *node = shark_fs->fs_inodes;
	int index_inode;
	int size;
	
	int name_size = 0;
	
	read_dentry_by_index(testing_index,temp);
	testing_index++;
	for(j = 0; j < NAME_FILE; j++) //get name 
	{
		buffer_name[j] = temp->filename[j];
		name_size++;
		if(temp->filename[j] == '\0'){
			break;
		}
	}
	int ret;
	ret = check_for_txt(buffer_name); //check for txt
	inodenum = temp->inode_index;
	index_inode = temp->inode_index;
	size = node[index_inode].length;
	read_data(inodenum,0,buffer,size); //read contents
	
	terminal_write(0,buffer,size);
	terminal_write(0,"\n",1);
	terminal_write(0, "filename: ",TESTBUFFER);
	terminal_write(0,buffer_name,name_size);
	terminal_write(0,"\n",1);
}
/*
* int check_for_txt(char *buffer)
*   Inputs:buffer
*   Return Value:1 if it is a text file, 0 otherwise
*	Function: test to check if we are looking at a text file
*/
int check_for_txt(char *buffer)
{
	int i;
	for(i = 0; i < NAME_FILE; i++)
	{
		if((buffer[i] == '.') && (buffer[i + 1] == 't') && (buffer[i + 2] == 'x'))
			return 1;
		if(buffer[0] == '.')
			return 1;
		if(buffer[i] == '\0'){
			return 0;
		}
	}
	return 0;
}

/*
* void init_write_data()
*   Inputs:none
*   Return Value:none
*	Function: initiailizes our write stuct so we know where we can create files
*/
void init_write_data(){
	int i,j;
	int fail = 0;
	//find available dentries
	for(i = 0; i < 63; i++){
		if(shark_fs->fs_dentries[i].filename[0] == '\0'){
			shark_write_data->dentries_filled[i] = 0;
		}
		else{
			shark_write_data->dentries_filled[i] = 1;
		}
	}

	//find available inodes
	for(i = 0; i < 64; i++){
		if(shark_fs->fs_inodes[i].length == 0){
			shark_write_data->inodes_filled[i] = 0;
		}
		else{
			shark_write_data->inodes_filled[i] = 1;
		}
	}

	//find available data blocks
	for(i = 0; i < ONEKILOBYTE; i++){
		fail = 0;
		for(j = 0; j < FOUR_KILOBYTES; j++){
			if(shark_fs->fs_data_blocks[i].data[j] != '\0'){
				fail = 1;
				break;
			}
		}
		if(fail == 0){
			shark_write_data->data_blocks_filled[i] = 0;
		}
		else{
			shark_write_data->data_blocks_filled[i] = 1;
		}
	}
	//now have available data
}


/*
* void write_to_file(const void *buf, int nbytes, int new_dentry_idx, int new_inode_idx, int new_data_block_idx)
*   Inputs: buf - buf to write, nbytes - bytes to write , idx's - filesystem indexes to write
*   Return Value: none
*	Function: writes to the file 
*/
void write_to_file(const void *buf, int nbytes, int new_dentry_idx, int new_inode_idx, int new_data_block_idx){
	int i;

	shark_fs->fs_dentries[new_dentry_idx].file_type = 2;
	shark_fs->fs_dentries[new_dentry_idx].inode_index = new_inode_idx;

	//update inode
	shark_fs->fs_inodes[new_inode_idx].length = nbytes;
	shark_fs->fs_inodes[new_inode_idx].data_blocks[0] = new_data_block_idx;

	//copy buffer into the data block
	for(i = 0; i < nbytes; i++){
		shark_fs->fs_data_blocks[new_data_block_idx].data[i] = *(uint8_t *)(buf + i);
	}

	shark_write_data->dentries_filled[new_dentry_idx] = 1;
	shark_write_data->inodes_filled[new_inode_idx] = 1;
	shark_write_data->data_blocks_filled[new_data_block_idx] = 1;
}

