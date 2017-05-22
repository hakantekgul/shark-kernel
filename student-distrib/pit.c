#include "pit.h"

/* Driver for Programmable Interval Timer */

/////// http://www.osdever.net/bkerndev/Docs/pit.htm //////////////////

static volatile int pit_interrupt;
static volatile int ticks = 0;
int seconds = 0;
static volatile int minute = 0;
static volatile int second = 0;
static volatile int minute1 = 0;
static volatile int minute2 = 0;
static volatile int minute3 = 0;
static volatile int minute4 = 0;
/*
*   pit_open(const uint8_t* filename)
*   Inputs: file from filesystem but not useful for RTC.
*   Return Value: 0 on success
*	Function: open function for the PIC driver. Enables RTC driver. Set the flag to 0.
*/
int32_t pit_open(const uint8_t* filename)
{
	pit_interrupt = 0;

	return 0;
}

/*
*   pit_close(int32_t fd)
*   Inputs: file descriptor that closes PIC
*   Return Value: 0 on success
*	Function: never close the PIC!
*/
int32_t pit_close(int32_t fd)
{
	//we never want to close PIT as scheduling should always be ON.
	return 0;
}

/*
*   pit_read(int32_t fd, void* buf, int32_t nbytes)
*   Inputs: file descriptor, video memory buffer, and number of bytes as an argument for real time clock
*   Return Value: 0 on success
*	Function: Return 0 when an interrupt happens and spin until it happens with an interrupt flag. 
*/
int32_t pit_read(int32_t fd, void* buf, int32_t nbytes)
{
	//if the flag is 1, clear it.
	if(pit_interrupt == 1)
		pit_interrupt = 0;
	while(pit_interrupt == 0) 
	{
		//do nothing, just wait
		//it will break the loop when it receives an interrupt
	}
	return 0;
}

/*
*   pit_write(int32_t fd, const void* buf, int32_t nbytes)
*   Inputs: file pointer, constant buffer, and number of bytes as an argument for real time clock
*   Return Value: NONE
*	Function: We always want the same rate when we init PIT - 40 millisecond
*/
int32_t pit_write(int32_t fd, const void* buf, int32_t nbytes)
{
	//we never want to change the interrupt timer freq or divisor for scheduling
	//quantum should be constant for scheduling
	return 0;
}

/*
*   init_pit()
*   Inputs: NONE
*   Return Value: 0 on success
*	Function: initializes IRQ0 on PIC and sets the freq to 25 Hz - 40 milliseconds
*/
int32_t init_pit(int channel)
{
	int freq;
	int divisor;

	freq = FIFTY_HZ; //convert 40 millisecond interrupt to frequency
	divisor = DIVISOR_BASE / freq; //get divisor value for PIT

	

	if(channel == 0)
	{
		outb(CMD_BYTE,CMD_MODE_REG);
		outb(divisor & LOW_BYTE_MASK, CHANNEL_0); //set low byte of divisor
		outb(divisor >> SHIFT_EIGHT, CHANNEL_0);  //set high byte of divisor
	}
	else if(channel == 2)
	{
		outb(CMD_BYTE,CMD_MODE_REG);
		outb(divisor & LOW_BYTE_MASK, CHANNEL_2); //set low byte of divisor
		outb(divisor >> SHIFT_EIGHT, CHANNEL_2); 
	}

	enable_irq(IRQ_PIT); //enable IRQ 0 <--> PIT

	return 0;
}

/*
*   pit_interrupt_handler()
*   Inputs: NONE
*   Return Value: 0 on success
*	Function: interrupt handler for PIC. Scheduling depends on this function.
*/
int32_t pit_interrupt_handler()
{
	cli();
	pit_interrupt = 1; //set the flag
//	send_eoi(IRQ_PIT); //send EOI
	int counter;
	counter = 0;
	ticks++;
	int p1,p2,p3;

	p1 = 0;
	p2 = 0;
	p3 = 0;
	
	seconds++;
	minute = seconds / MINUTE_SIXTY;
	minute2 = minute / TEN_TIME;
	minute1 = minute % TEN_TIME; 
		

	if(minute1 > TEN_TIME-1)
	{
		minute1 = 0;
	}
	if(minute2 > MINUTE_TIME)
	{
		minute2 = 0;
		minute3++;
		seconds = 0;
	}
	if(minute3 > TEN_TIME-1)
	{
		minute3 = 0;
		minute4++;
	}
		

	update_time(minute1,minute2,minute3,minute4);

	send_eoi(IRQ_PIT);


	sched();

	return 0;
}




