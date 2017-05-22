#include "rtc.h"

static volatile int rtc_interrupt_occured;
volatile int test_freq = 2;
volatile int col_counter = 0;

/*
*   init_rtc()
*   Inputs: NONE
*   Return Value: NONE
*	Function: initializes IRQ8 on PIC manually and starts generating periodic interrupts 
*/
void init_rtc()
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	char current_b; //to get the current value of B 
	int bit6 = BIT_SIX; //to get bit 6 of register B 
	
	outb(RTC_PORT_1, inb(RTC_PORT_1) | NMI_DISABLE); //disable NMI
	outb(REGISTER_B, RTC_PORT_1);   // select Register B and disable NMI 
	current_b = inb(CMOS); //read the current value of B
	outb(REGISTER_B, RTC_PORT_1); //set the index again 
	outb(current_b | bit6, CMOS); //Turn on bit 6 of register 
	outb(RTC_PORT_1, inb(RTC_PORT_1) & NMI_ENABLE);//enable iterrupts
	
	rtc_set_freq(INIT_FREQ);

	return;
}

/*
*   handle_rtc_interrupt()
*   Inputs: NONE
*   Return Value: NONE
*	Function: interrupt handler for RTC. See comments below to get interrupt again 
*/
void handle_rtc_interrupt()
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	//What is important is that if register C is not read after an IRQ 8, then the interrupt will not happen again. 
	//So, even if you don't care about what type of interrupt it is, 
	//just attach this code to the bottom of your IRQ handler to be sure you get another interrupt.
	
	rtc_interrupt_occured = 1;
	outb(REGISTER_C, RTC_PORT_1); //select register C
	inb(CMOS);                    //just throw away contents
	send_eoi(IRQ_RTC);            //send end of interrupt signal 
	
	return;
}

/*
*   rtc_set_freq(int rate)
*   Inputs: rate to be set ----> freq = 32768 >> (rate - 1)
*   Return Value: 0 on success, -1 on failure.
*	Function: sets the frequency rate for RTC where rate is given  
*/
int rtc_set_freq(int rate)
{ 
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////

	if(rate < 6) //rate 6 means 1024 Hz
	{
		return -1;
	}
	rate &= 0x0F;			// rate must be above 6 and not over 15
	
	outb(RTC_PORT_1, inb(RTC_PORT_1) | NMI_DISABLE); //disable NMI

	outb(REGISTER_A, RTC_PORT_1);		// set index to register A, disable NMI
	char prev=inb(CMOS);	// get initial value of register A
	outb(REGISTER_A, RTC_PORT_1);		// reset index to A
	outb((prev & BOTTOM4) | rate, CMOS); //write only our rate to A. Note, rate is the bottom 4 bits.

	outb(RTC_PORT_1, inb(RTC_PORT_1) & NMI_ENABLE);//enable interrupts
	

	return 0;
}

/*
*   rtc_open(const uint8_t* filename)
*   Inputs: file from filesystem but not useful for RTC.
*   Return Value: 0 on success
*	Function: open function for the real time clock for RTC driver. Enables RTC driver. Set frequency to 2 Hz.
*/
int32_t rtc_open(const uint8_t* filename)
{
	rtc_interrupt_occured = 0;
	
	rtc_set_freq(INIT_FREQ);  //Set the RTC to initial frequency of 2 Hz.
	
	return 0;
}

/*
*   rtc_close(int32_t fd)
*   Inputs: file pointer that closes real time clock
*   Return Value: 0 on success
*	Function: closes function for the real time clock for RTC driver.
*/
int rtc_close(int32_t fd)
{
	//we never want to close the RTC as it should always generate periodic interrupts.
	return 0;
}

/*
*   rtc_read(int32_t fd, void* buf, int32_t nbytes)
*   Inputs: file pointer, video memory buffer, and number of bytes as an argument for real time clock
*   Return Value: 0 on success
*	Function: Return 0 when an interrupt happens and spin until it happens with an interrupt flag. 
*/
int rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	// set a flag and wait until the interrupt handler clears it, then return 0 // FROM DOCUMENTATION: RTDC PLEASE!
	if(rtc_interrupt_occured == 1)
		rtc_interrupt_occured = 0; //switched this because TA said so to fix our mistake in 3.2

	while(rtc_interrupt_occured == 0)
	{
		//do nothing, just wait
		//it will break the loop when it receives an interrupt
	}
	

	return 0;
}

/*
*   rtc_write(int32_t fd, const void* buf, int32_t nbytes)
*   Inputs: file pointer, constant buffer, and number of bytes as an argument for real time clock
*   Return Value: NONE
*	Function: Sets the interrupt rate with accepted 4 bytes
*/
int rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	int freq; //frequency to be calculated
	int ret;  //return value 
	int rate; //rate argument for helper function
	if(nbytes == 4) //4 from documentation 
	{
		freq = *((int32_t *)buf); //get the frequency from buffer

		rate = (INIT_FREQ + 1) - log_base_2(freq); //calculate rate

		ret = rtc_set_freq(rate); //set to found frequency 
		return ret;
	}

	return -1;
}

/*
*   log_base_2(int number)
*   Inputs: Real number that will be used as log base 2
*   Return Value: log base 2 of the input number
*	Function: Calculates the log base 2 of a number to find the right rate for RTC
*/
int log_base_2(int number)
{
	int result = 0;
	while((number/2) != 1) //divide by two until it hits 1 
	{
		number = number / 2;
		result++;
	}

	result++; //increment it one last time for the right result

	return result;
}

/*
*   rtc_test()
*   Inputs: NONE
*   Return Value: NONE
*	Function: Tester for RTC driver for open,write and close. Increases frequency by 2 every time 
*   it is called.
*/
void rtc_test()
{
	//printf("RTC DRIVER TESTING INTERFACE ENABLED! \n \n");
	//change this to get half points back!
	rtc_open(0);
	rtc_write(0,(int*)&test_freq,4);
	test_freq = test_freq * 2;
	if(test_freq > MAX_FREQ) //1024 is the user limit freq 
		test_freq = 2;
	rtc_close(0);

}

/*
*   rtc_read_test()
*   Inputs: NONE
*   Return Value: NONE
*	Function: Tester for RTC driver for read fucntion. Checks if rtc can read interrupts. 
*/
void rtc_read_test()
{
	rtc_read(0,0,0);
	return;
}







