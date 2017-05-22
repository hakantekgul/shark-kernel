/* mouse.c */

//DRIVER AND INITIALIZATION OF MOUSE

#include "mouse.h"

#define VIDEO 0xB8000
static int prev_x;
static int prev_y;


/*
* void init_mouse()
*   Inputs: NONE
*   Return Value: NONE
*   Function: Initialize the mouse driver  
*/
void init_mouse()
{
  prev_x = 1;
  prev_y = 1;
  uint8_t* mouse_user_video;
  

	//mouse_wait(1);
    //enable the auxiliary mouse device 
	outb(0xA8,0x64);

	//mouse_wait(1);
    //get "compaq status byte"
	outb(0x20,0x64);

    //store the byte
	unsigned char status_byte;
	//mouse_wait(0);
    //enabling IRQ 12
	status_byte = (inb(0x60) | 2);

	//mouse_wait(1);
    //tell the controller we will be sending a status byte
	outb(0x60, 0x64);

	//mouse_wait(1);
    //send the modified status byte 
	outb(status_byte, 0x60);

    //tell the mouse to use default settings for simplicity
	mouse_write(0xF6,0,0,0);
    //wait for the mouse to accept our command
	mouse_read(0,0,0);

    //enable the device at the end 
	mouse_write(0xF4,0,0,0);
	mouse_read(0,0,0);



    vidmap(&mouse_user_video);

    init_cursor();
}

/*
* void mouse_wait(unsigned char type)
*   Inputs: type of wait as an integer - 0 for data 1 for signaling
*   Return Value: NONE
*   Function: Wait function for the mouse to be able to send or receive data  
*/
void mouse_wait(unsigned char type)
{ 
    unsigned int _time_out = 100000; //some big number so that mouse will wait
    if(type == 0)
    {
        while(_time_out--) //Data //wait until number reaches zero
        {
            if((inb(0x64) & 1)==1) //if we get ack from mouse, return
            {
                return;
            }
        }
        return;
    }
    else
    {
        while(_time_out--) //Signal
        {
            if((inb(0x64) & 2)==0) //ack from mouse, return 
            {
                return;
            }
        }
    return;
  }
}

/*
* void mouse_handler()
*   Inputs: NONE
*   Return Value: NONE
*   Function: Interrupt handler for mouse device 
*/
void mouse_handler()
{
  //printf("mouse interrupt received\n");

  static unsigned char cycle = 0;    //for bytes
  static char mouse_bytes[3];        //to restore bytes
  mouse_bytes[cycle++] = inb(0x60);  //get bytes
  
 
  if (cycle == 3) 
  { 
    int x = mouse_bytes[0];
    int y = mouse_bytes[1];
    cycle = 0; // reset the counter
    move_mouse(x,y);
    if (mouse_bytes[2] & 0x4)
    {
      //printf("Middle button is pressed!\n");
    }
    if (mouse_bytes[2] & 0x2) //right click
    {
        
    }
    if (mouse_bytes[2] & 0x1) //left click
    {
        if(prev_y == 24 && (prev_x > 18 && prev_x < 34))
            before_swap(1);
        if(prev_y == 24 && (prev_x > 34 && prev_x < 49))
            before_swap(2);
        if(prev_y == 24 && (prev_x > 49 && prev_x < 63))
            before_swap(3);
        if(prev_y == 24 && (prev_x > 68 && prev_x < 72))
           shutdown();
    }
  }

  send_eoi(12);
  
  return;
}

/*
* void mouse_open(const uint8_t* filename)
*   Inputs: filename for opening mouse
*   Return Value: 0 on success 
*   Function: Open function for mouse driver  
*/
int32_t mouse_open(const uint8_t* filename)
{

    init_mouse();

    return 0;
}

/*
* void mouse_close(fd)
*   Inputs: file descriptor
*   Return Value: 0 on success 
*   Function: Close function for mouse driver  
*/
int mouse_close(int32_t fd)
{
    disable_irq(12);

    return 0;
}

/*
* void mouse_write(unsigned char a_write)
*   Inputs: Data to write to the port of mouse
*   Return Value: NONE
*   Function: Write function for the mouse to be able to send a command  
*/
void mouse_write(unsigned char a_write, int32_t fd, const void* buf, int32_t nbytes)
{
    //Wait to be able to send a command
    mouse_wait(1);
    //Tell the mouse we are sending a command
    outb(0xD4, 0x64);
    //Wait for the final part
    mouse_wait(1);
    //Finally write
    outb(a_write, 0x60);
}

/*
* unsigned char mouse_read()
*   Inputs: NONE
*   Return Value: character read from mouse
*   Function: Gets response from mouse  
*/
unsigned char mouse_read(int32_t fd, void* buf, int32_t nbytes)
{
    //Get response from mouse
    mouse_wait(0);
    return inb(0x60);
}

/*
* void init_cursor()
*   Inputs: NONE
*   Return Value: NONE
*   Function: initializes cursor on screen at video mem 1,1  
*/
void init_cursor()
{
    return;

}

/*
* void move_mouse(int x, int y)
*   Inputs: x and y movement values from mouse packets 
*   Return Value: NONE
*   Function: moves mouse cursor on screen  
*/
void move_mouse(int x, int y)
{
    //save the character on screen 
    putc_mouse(whole_screen_buffer[prev_y * 80 + prev_x],prev_x,prev_y,0x3);
    //status_bar();
    y = y * -1; //bug fixed

    x = x % 10; //slow down the values for smoother movement
    y = y % 10;

    //check screen limits so that it will not page fault
    if(x < 0 && (prev_x + x) < 0)
        x = 0;
    if(y < 0 && (prev_y + y) < 0)
        y = 0;
    
    if((prev_x + x) > 79)
        x = 0;

    if((prev_y + y) > 24)
        y = 0;


    prev_x = x + prev_x; //update x and y global values
    prev_y = y + prev_y;

    

    status_bar();
    putc_mouse(219,prev_x,prev_y,0x3); //put the cursor on screen
}









