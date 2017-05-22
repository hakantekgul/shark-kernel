/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab
 */

#include "lib.h"
#define VIDEO 0xB8000
#define VIDEO_END 0xB8FA0
#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7
#define ONE_LINE_BYTES 160
#define eight 8
#define STATUS_START 80 * 24
#define STATUS_END   80 * 25
#define ASCII_INDEX  48
#define SHUTDOWN     0xf4
#define TERMINAL1    0xB9000
#define TERMINAL2    0xBA000
#define TERMINAL3    0xBB000

static int screen_x;
static int screen_y;
static int mouse_x = 0;
static int mouse_y = 0;
static char* video_mem = (char *)VIDEO;
  
/*
 * void shutdown()
 *   DESCRIPTION: shuts down qemu and closes it.
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Qemu is closed. 
 */                          
void shutdown()
{
	//char buffer[50] = "Are you sure you want to shutdown the computer?";
	outb(0x00, SHUTDOWN);
	return;
}           

/*
 * void update_time(int second0, int second1, int minute0, int minute1)
 *   DESCRIPTION: Updates time on status bar each second  
 *   INPUTS: 4 digits for timer
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Timer is updated on status bar
 */              
void update_time(int second0, int second1, int minute0, int minute1)
{
	int i = 0;
	int j = 0;
	for(i = STATUS_START, j = 0; i < STATUS_END; i++, j++) 
	{
        if(j == 79)
        	*(uint8_t *)(video_mem + (i << 1)) = second0+ASCII_INDEX;
        if(j == 78)
        	*(uint8_t *)(video_mem + (i << 1)) = second1+ASCII_INDEX;
        if(j == 77)
        	*(uint8_t *)(video_mem + (i << 1)) = ':';
        if(j == 76)
        	*(uint8_t *)(video_mem + (i << 1)) = minute0+ASCII_INDEX;
        if(j == 75)
        	*(uint8_t *)(video_mem + (i << 1)) = minute1+ASCII_INDEX;
    }
}

/*
 * void status_bar()
 *   DESCRIPTION: Puts status bar with color on video memory, last row
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: Status bar is shown on video mem
 */ 
void status_bar()
{
	int i = 0;
	int j = 0;
	int num = get_terminal_number();
	uint8_t buffer[64] = "current terminal:   |  terminal 1  |  terminal 2  |  terminal 3";
	for(i = STATUS_START, j = 0; i < STATUS_END; i++, j++) 
	{
        *(uint8_t *)(video_mem + (i << 1) + 1) = 0x60;
        if(j<64)
        	*(uint8_t *)(video_mem + (i << 1)) = buffer[j];
        if(j == 17)
        	*(uint8_t *)(video_mem + (i << 1)) = num + ASCII_INDEX;
        if(j == 68)
        	*(uint8_t *)(video_mem + (i << 1)) = 'H';
        if(j == 69)
        	*(uint8_t *)(video_mem + (i << 1)) = 'A';
        if(j == 70)
        	*(uint8_t *)(video_mem + (i << 1)) = 'L';
        if(j == 71)
        	*(uint8_t *)(video_mem + (i << 1)) = 'T';
    }

}

/*
 * uint32_t * get_terminal_address()
 *   DESCRIPTION: gets terminal address from pointer dereferencing
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: pointer of terminal address
 *   SIDE EFFECTS: NONE
 */ 
uint32_t * get_terminal_address(){
	uint32_t *address;
	if(curr_term == terminal1){
		address = (uint32_t *)TERMINAL1;
	}
	else if(curr_term == terminal2){
		address = (uint32_t *)TERMINAL2;
	}
	else if(curr_term == terminal3){
		address = (uint32_t *)TERMINAL3;
	}
	return address;
}

/*
 * uint32_t * get_terminal_number()
 *   DESCRIPTION: gets terminal number from curr_term
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: current terminal number
 *   SIDE EFFECTS: NONE
 */
uint32_t get_terminal_number(){
	unsigned int num;
	if(curr_term == terminal1){
		num = 1;
	}
	else if(curr_term == terminal2){
		num = 2;
	}
	else if(curr_term == terminal3){
		num = 3;
	}
	return num;
}

/*
 * uint32_t * term_num_to_address()
 *   DESCRIPTION: gets the address to write to in terminal
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: pointer to terminal address to write to
 *   SIDE EFFECTS: NONE
 */
uint32_t * term_num_to_address(int num){
	uint32_t *address;
	if(num == 1){
		address = (uint32_t *)TERMINAL1;
	}
	else if (num == 2){
		address = (uint32_t *)TERMINAL2;
	}
	else if(num == 3){
		address = (uint32_t *)TERMINAL3;
	}
	return address;
}

/*
 * void init_keyboard_map()
 *   DESCRIPTION: initializes keyboard mapping
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: NONE
 */
void init_kboard_map(){

	//keycode for alt shift funtion keys etc. 
	kboard_map[59] = 0x1;
	kboard_map[60] = 0x1;
	kboard_map[61] = 0x1;
	kboard_map[42] = 0x1;
	kboard_map[54] = 0x1;
	kboard_map[89] = 0x1;
	kboard_map[170] = 0x1;
	kboard_map[182] = 0x1;
	kboard_map[29] = 0x1;
	kboard_map[157] = 0x1;
	kboard_map[240] = 0x1;
	kboard_map[56] = 0x1;
	kboard_map[184] = 0x1;
	kboard_map[73] = 0x1;
	kboard_map[72] = 0x1;
	kboard_map[80] = 0x1;
}

/*
 * void boot_shark
 *   DESCRIPTION: boots the OS with shark ASCII ART
 *   INPUTS: NONE
 *   OUTPUTS: NONE
 *   RETURN VALUE: NONE
 *   SIDE EFFECTS: NONE
 */
void boot_shark()
{
	int i,j;
	printf("          ");
	printf("                        ,\n");
	printf("          ");
	printf("                       { \\ \n");
	printf("          ");
	printf("                      {   `\\ \n");
	printf("          ");
	printf("                     {  .-'""'--.\n");
	printf("          ");
	printf("                    {.'       00 \n");
	printf("          ");
	printf("                   /`    /\\_______')\n");
	printf("          ");
	printf("                  /  ((  \\ \\/\\/\\/\\/\n");
	printf("          ");
	printf("                 /        `^^^^/\n");
	printf("          ");
	printf("                |           '/{`\\\n");
	printf("          ");
	printf("                | |        `/{__|\n");
	printf("          ");
	printf("                |  |   |.-'//,\n");
	printf("          ");
	printf("                 |  |   | // |\n");
	printf("          ");
	printf("                  |  |___|/.'\n");
	printf("          ");
	printf("                   | |  //'\\\n");
	printf("          ");
	printf("                  /  /| ` _/\n");
	printf("          ");
	printf("                 /  (  ```\n");
	printf("          ");
	printf("                '   ' \\\n");
	printf("          ");
	printf("              /__/V|___| \n");
	printf("\n\n");
	printf("                       __PROJECT SHARK__\n");
	printf("                 Shounak Hakan Arvind Raimi KERNEL\n");

	for(i = 0; i < 79; i++)
	{
		printf("#");
		for(j = 0; j < 4000000; j++)
		{
			int a = j * 78 * 89 * 23794;
			j++;
			a = a - 1;
		}
	}

	clear_screen();
}

/*
* void update_cursor();
*   Inputs: void
*   Return Value: none
*	Function: updates the location of the cursor on the screen
*/
void update_cursor(int idx){
	//taken from osdev
	screen_x = idx % TERMINAL_WIDTH;
	screen_y = idx / TERMINAL_WIDTH;
	unsigned short position = (screen_y * TERMINAL_WIDTH) + screen_x;
    // cursor LOW port to vga INDEX register
    outb(0x0F, 0x3D4);
    outb((unsigned char)(position & 0xFF), 0x3D5);
    // cursor HIGH port to vga INDEX register
    outb(0x0E, 0x3D4);
    outb((unsigned char)((position >> eight) & 0xFF), 0x3D5);
}

/*
* void vertical_scroll();
*   Inputs: void
*   Return Value: none
*	Function: scrolls the screen by moving everything up 1 line
*/
void vertical_scroll(){
	//start at end of array, shift a ll up by 80.
	int i,x,y;

	//clear();
	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE-TERMINAL_WIDTH; i++){
		x = i % TERMINAL_WIDTH;
		y = i / TERMINAL_WIDTH;	
		if(whole_screen_buffer[i + TERMINAL_WIDTH] == '\n'){
			putc_terminal(0, x,y);
		}
		else{
			putc_terminal(whole_screen_buffer[i + TERMINAL_WIDTH],x,y);
		}
		whole_screen_buffer[i] = whole_screen_buffer[i + TERMINAL_WIDTH];
	}

	whole_screen_buffer_index = WHOLE_SCREEN_BUFFER_SIZE - TERMINAL_WIDTH;
	for(i = WHOLE_SCREEN_BUFFER_SIZE - TERMINAL_WIDTH; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		x = i % TERMINAL_WIDTH;
		y = i / TERMINAL_WIDTH;
		whole_screen_buffer[i] = 0;
		putc_terminal(0,x,y);
	}
	screen_x = 0;
	screen_y = TERMINAL_HEIGHT - 1;
}

/*
* void clear_screen();
*   Inputs: void
*   Return Value: none
*	Function: clears the screen and sets the cursor to the 0 position
*/
void clear_screen(){
	int i,x,y;
	clear();
	uint8_t *addr = (uint8_t *)term_num_to_address(get_terminal_number());

	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE; i++){
		y = i / TERMINAL_WIDTH;
		x = i % TERMINAL_WIDTH;
		*(uint8_t *)(addr + ((TERMINAL_WIDTH*y + x) << 1)) = 0;
        *(uint8_t *)(addr + ((TERMINAL_WIDTH*(i / TERMINAL_WIDTH) + (i % TERMINAL_WIDTH)) << 1) + 1) = 0x3;
	}


	update_cursor(0);
	for(i = 0; i < WHOLE_SCREEN_BUFFER_SIZE - NUM_COLS; i++){
		if(i < BUFFER_SIZE){
			buffer[i] = 0;
		}
		whole_screen_buffer[i] = 0;
	}
	whole_screen_buffer_index = 0;
	buffer_index = -1;
	//terminal_write(1, "391OS> ", 7);
}

/*
* void do_backspace();
*   Inputs: void
*   Return Value: none
*	Function: removes the previous character from the screen and from the buffers
*/
void do_backspace(){
	int flag = 0;
	int x,y;
	uint8_t *addr = (uint8_t *)get_terminal_address();
	int num = get_terminal_number();
	if(buffer_index == -1 || buffer[buffer_index] == '\n'){	//cant backspace an enter
		return;
	}
	//edge case if we backspace onto previous line
	if(whole_screen_buffer_index % TERMINAL_WIDTH == 0){
		while(whole_screen_buffer[whole_screen_buffer_index - 1] == 0){
			    flag = 1;
			    if(whole_screen_buffer_index == 0){
			    	break;
			    }
			    whole_screen_buffer[whole_screen_buffer_index-1] = 0;         //set to 0
    			whole_screen_buffer_index--;
		}
	}
	

    if(flag == 0){
    	whole_screen_buffer[whole_screen_buffer_index-1] = 0;         //set to 0
    	whole_screen_buffer_index--;
    }
    x = whole_screen_buffer_index % TERMINAL_WIDTH;
	y = whole_screen_buffer_index / TERMINAL_WIDTH;
    if(buffer_index != -1){
    	buffer[buffer_index] = 0;
    	*(uint8_t *)(addr + ((TERMINAL_WIDTH*y + x) << 1)) = 0;
        *(uint8_t *)(addr + ((TERMINAL_WIDTH*(y) + (x)) << 1) + 1) = 0x3 + num;
    	buffer_index--;
    }
    screen_x = whole_screen_buffer_index % TERMINAL_WIDTH;
    screen_y = whole_screen_buffer_index / TERMINAL_WIDTH;
	putc(0);
}

/*
* void clear(void);
*   Inputs: void
*   Return Value: none
*	Function: Clears video memory
*/

void
clear(void)
{
    int32_t i;
    for(i=0; i<(NUM_ROWS*NUM_COLS) - NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{
	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	while(*buf != '\0') {
		switch(*buf) {
			case '%':
				{
					int32_t alternate = 0;
					buf++;

format_char_switch:
					/* Conversion specifiers */
					switch(*buf) {
						/* Print a literal '%' character */
						case '%':
							putc('%');
							break;

						/* Use alternate formatting */
						case '#':
							alternate = 1;
							buf++;
							/* Yes, I know gotos are bad.  This is the
							 * most elegant and general way to do this,
							 * IMHO. */
							goto format_char_switch;

						/* Print a number in hexadecimal form */
						case 'x':
							{
								int8_t conv_buf[64];
								if(alternate == 0) {
									itoa(*((uint32_t *)esp), conv_buf, 16);
									puts(conv_buf);
								} else {
									int32_t starting_index;
									int32_t i;
									itoa(*((uint32_t *)esp), &conv_buf[8], 16);
									i = starting_index = strlen(&conv_buf[8]);
									while(i < 8) {
										conv_buf[i] = '0';
										i++;
									}
									puts(&conv_buf[starting_index]);
								}
								esp++;
							}
							break;

						/* Print a number in unsigned int form */
						case 'u':
							{
								int8_t conv_buf[36];
								itoa(*((uint32_t *)esp), conv_buf, 10);
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a number in signed int form */
						case 'd':
							{
								int8_t conv_buf[36];
								int32_t value = *((int32_t *)esp);
								if(value < 0) {
									conv_buf[0] = '-';
									itoa(-value, &conv_buf[1], 10);
								} else {
									itoa(value, conv_buf, 10);
								}
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a single character */
						case 'c':
							putc( (uint8_t) *((int32_t *)esp) );
							esp++;
							break;

						/* Print a NULL-terminated string */
						case 's':
							puts( *((int8_t **)esp) );
							esp++;
							break;

						default:
							break;
					}

				}
				break;

			default:
				putc(*buf);
				break;
		}
		buf++;
	}

	return (buf - format);
}

/*
* int32_t puts(int8_t* s);
*   Inputs: int_8* s = pointer to a string of characters
*   Return Value: Number of bytes written
*	Function: Output a string to the console 
*/

int32_t
puts(int8_t* s)
{
	register int32_t index = 0;
	while(s[index] != '\0') {
		putc(s[index]);
		index++;
	}

	return index;
}

/*
* void putc(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the console 
*/

void
putc(uint8_t c)
{
    if(c == '\n' || c == '\r') {
        screen_y++;
        screen_x=0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y + screen_x) << 1) + 1) = 0x1;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
}
/*
* void putc_mouse(uint8_t c, int x, int y);
*   Inputs: uint_8* c = character to print, x and y coordinates
*   Return Value: void
*	Function: Output a character for mouse cursor 
*/

void
putc_mouse(uint8_t c, int x, int y, int attribute)
{
	mouse_x = x;
	mouse_y = y;
	int num = get_terminal_number();
	if(attribute == 0x60)
		num = 0;
    
    *(uint8_t *)(video_mem + ((NUM_COLS*mouse_y + mouse_x) << 1)) = c;
    *(uint8_t *)(video_mem + ((NUM_COLS*mouse_y + mouse_x) << 1) + 1) = attribute + num;
    mouse_x++;
    mouse_x %= NUM_COLS;
    mouse_y = (mouse_y + (mouse_x / NUM_COLS)) % NUM_ROWS;
    
}
/*
* void putc_terminal(uint8_t c);
*   Inputs: uint_8* c = character to print, x, y
*   Return Value: void
*	Function: Output a character to the console for terminal write 
*/

void
putc_terminal(uint8_t c, int x, int y)
{
	uint8_t *addr;
	addr = (uint8_t *)term_num_to_address(get_terminal_number());
	screen_x = x;
	screen_y = y;
    if(c == '\n' || c == '\r') {
        whole_screen_buffer_index = (whole_screen_buffer_index + TERMINAL_WIDTH) - (whole_screen_buffer_index % TERMINAL_WIDTH);
        if(whole_screen_buffer_index >= WHOLE_SCREEN_BUFFER_SIZE){
       		vertical_scroll();
    	}
    	screen_y++;
        screen_x=0;
    } else {
        *(uint8_t *)(addr + ((NUM_COLS*screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(addr + ((NUM_COLS*screen_y + screen_x) << 1) + 1) = 0x3 + get_terminal_number();
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
        putc_running_terminal(c,x,y);

    }
}

/*
* void putc_running_terminal(uint8_t c);
*   Inputs: uint_8* c = character to print, x, y
*   Return Value: void
*	Function: Output a character to the video memory 
*/

void
putc_running_terminal(uint8_t c, int x, int y)
{
	//check with pcb_scheduling to pick video mem?
	
	if(pcb_scheduling == NULL){
		//write to video memory
		*(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1)) = c;
		*(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1) + 1) = 0x3 + get_terminal_number();
	}
	else if((get_terminal_number() == pcb_scheduling->terminal_number)){
		//write to video memory
		*(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1)) = c;
		*(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1) + 1) = 0x3 + get_terminal_number();
	}
	//whole_screen_buffer_index++;
//	buffer_index++;
	return;

}

/*
* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
*   Inputs: uint32_t value = number to convert
*			int8_t* buf = allocated buffer to place string in
*			int32_t radix = base system. hex, oct, dec, etc.
*   Return Value: number of bytes written
*	Function: Convert a number to its ASCII representation, with base "radix"
*/

int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
	static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int8_t *newbuf = buf;
	int32_t i;
	uint32_t newval = value;

	/* Special case for zero */
	if(value == 0) {
		buf[0]='0';
		buf[1]='\0';
		return buf;
	}

	/* Go through the number one place value at a time, and add the
	 * correct digit to "newbuf".  We actually add characters to the
	 * ASCII string from lowest place value to highest, which is the
	 * opposite of how the number should be printed.  We'll reverse the
	 * characters later. */
	while(newval > 0) {
		i = newval % radix;
		*newbuf = lookup[i];
		newbuf++;
		newval /= radix;
	}

	/* Add a terminating NULL */
	*newbuf = '\0';

	/* Reverse the string and return */
	return strrev(buf);
}

/*
* int8_t* strrev(int8_t* s);
*   Inputs: int8_t* s = string to reverse
*   Return Value: reversed string
*	Function: reverses a string s
*/

int8_t*
strrev(int8_t* s)
{
	register int8_t tmp;
	register int32_t beg=0;
	register int32_t end=strlen(s) - 1;

	while(beg < end) {
		tmp = s[end];
		s[end] = s[beg];
		s[beg] = tmp;
		beg++;
		end--;
	}

	return s;
}

/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlen(const int8_t* s)
{
	register uint32_t len = 0;
	while(s[len] != '\0')
		len++;

	return len;
}

/*
* void* memset(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive bytes of pointer s to value c
*/

void*
memset(void* s, int32_t c, uint32_t n)
{
	c &= 0xFF;
	asm volatile("                  \n\
			.memset_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memset_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memset_aligned \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memset_top     \n\
			.memset_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     stosl           \n\
			.memset_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memset_done    \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%edx       \n\
			jmp     .memset_bottom  \n\
			.memset_done:           \n\
			"
			:
			: "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_word(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set lower 16 bits of n consecutive memory locations of pointer s to value c
*/

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosw           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_dword(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive memory locations of pointer s to value c
*/

void*
memset_dword(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosl           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memcpy(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of copy
*			const void* src = source of copy
*			uint32_t n = number of byets to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of src to dest
*/

void*
memcpy(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			.memcpy_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memcpy_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memcpy_aligned \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memcpy_top     \n\
			.memcpy_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     movsl           \n\
			.memcpy_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memcpy_done    \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%edx       \n\
			jmp     .memcpy_bottom  \n\
			.memcpy_done:           \n\
			"
			:
			: "S"(src), "D"(dest), "c"(n)
			: "eax", "edx", "memory", "cc"
			);

	return dest;
}

/*
* void* memmove(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of move
*			const void* src = source of move
*			uint32_t n = number of byets to move
*   Return Value: pointer to dest
*	Function: move n bytes of src to dest
*/

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			cmp     %%edi, %%esi    \n\
			jae     .memmove_go     \n\
			leal    -1(%%esi, %%ecx), %%esi    \n\
			leal    -1(%%edi, %%ecx), %%edi    \n\
			std                     \n\
			.memmove_go:            \n\
			rep     movsb           \n\
			"
			:
			: "D"(dest), "S"(src), "c"(n)
			: "edx", "memory", "cc"
			);

	return dest;
}

/*
* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*			uint32_t n = number of bytes to compare
*	Return Value: A zero value indicates that the characters compared 
*					in both strings form the same string.
*				A value greater than zero indicates that the first 
*					character that does not match has a greater value 
*					in str1 than in str2; And a value less than zero 
*					indicates the opposite.
*	Function: compares string 1 and string 2 for equality
*/

int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for(i=0; i<n; i++) {
		if( (s1[i] != s2[i]) ||
				(s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}


/*
* int32_t filecmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*	Return Value: A zero value indicates that the characters compared 
*					in both strings form the same string.
*				1 if they are not equal
*	Function: compares string 1 and string 2 for equality
*/

int32_t
filecmp(const uint8_t* s1, const uint8_t* s2)
{
	int32_t i = 0;
	//if(strlen((int8_t*)s1) > 32)
		//return 1;
	while(i < 32){
		if(s1[i] == '\0' && s2[i] == '\0'){
			break;
		}
		if((s1[i] == '\0' && s2[i] != '\0') || (s1[i] != '\0' && s2[i] == '\0')){
			return 1;
		}
		if(s1[i] != s2[i]){
			return 1;
		}
		i++;
	}
	return 0;

}


/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string
*/

int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
	int32_t i=0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
	int32_t i=0;
	while(src[i] != '\0' && i < n) {
		dest[i] = src[i];
		i++;
	}

	while(i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}

/*
* void test_interrupts(void)
*   Inputs: void
*   Return Value: void
*	Function: increments video memory. To be used to test rtc
*/

void
test_interrupts(void)
{
	int32_t i;
	for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
		video_mem[i<<1]++;
	}
}
