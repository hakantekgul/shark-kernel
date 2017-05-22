#include "idt.h"
#include "rtc.h"


/*  
*   init_IDT()
*   Inputs: NONE
*   Return Value: 0 no success 
*	Function: Sends an end of interrupt signal to the PIC. 
*/
 int init_IDT(){
	//return 0 on success, -1 on fail


	lidt(idt_desc_ptr);    //load the interrupt descriptor table 

	idt_desc_t generic_interrupt; //declare a generic interrupt to handle all exceptions

	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////

	generic_interrupt.seg_selector = KERNEL_CS;
	generic_interrupt.dpl = 0;                   //privelege level is for kernel EXCEPT SYSTEM CALLS
	generic_interrupt.reserved1 = 1;
	generic_interrupt.reserved2 = 1;
	generic_interrupt.reserved0 = 0;
	generic_interrupt.reserved3 = 0;
	generic_interrupt.reserved4 = 0;
	generic_interrupt.size = 1;
	generic_interrupt.present = 1;

	idt_desc_t e0 = generic_interrupt; //divide by zero 
	SET_IDT_ENTRY(e0, exception_0);//set the idt entry with corresponding handler
	idt[0] = e0;

	idt_desc_t e1 = generic_interrupt;
	SET_IDT_ENTRY(e1, exception_1);//set the idt entry with corresponding handler
	idt[1] = e1;

	idt_desc_t e2 = generic_interrupt;
	SET_IDT_ENTRY(e2, exception_2);//set the idt entry with corresponding handler
	idt[2] = e2;

	idt_desc_t e3 = generic_interrupt;
	SET_IDT_ENTRY(e3, exception_3);//set the idt entry with corresponding handler
	idt[3] = e3;

	idt_desc_t e4 = generic_interrupt;
	SET_IDT_ENTRY(e4, exception_4);//set the idt entry with corresponding handler
	idt[4] = e4;


	idt_desc_t e5 = generic_interrupt;
	SET_IDT_ENTRY(e5, exception_5);//set the idt entry with corresponding handler
	idt[5] = e5;

	idt_desc_t e6 = generic_interrupt;
	SET_IDT_ENTRY(e6, exception_6);//set the idt entry with corresponding handler
	idt[6] = e6;

	idt_desc_t e7 = generic_interrupt;
	SET_IDT_ENTRY(e7, exception_7);//set the idt entry with corresponding handler
	idt[7] = e7;

	idt_desc_t e8 = generic_interrupt;
	SET_IDT_ENTRY(e8, exception_8);//set the idt entry with corresponding handler
	idt[8] = e8;

	idt_desc_t e9 = generic_interrupt;
	SET_IDT_ENTRY(e9, exception_9);//set the idt entry with corresponding handler
	idt[9] = e9;

	idt_desc_t e10 = generic_interrupt;
	SET_IDT_ENTRY(e10, exception_10);  //set the idt entry with corresponding handler
	idt[10] = e10;

	idt_desc_t e11 = generic_interrupt;
	SET_IDT_ENTRY(e11, exception_11);  //set the idt entry with corresponding handler
	idt[11] = e11;

	idt_desc_t e12 = generic_interrupt;
	SET_IDT_ENTRY(e12, exception_12);  //set the idt entry with corresponding handler
	idt[12] = e12;

	idt_desc_t e13 = generic_interrupt;
	SET_IDT_ENTRY(e13, exception_13);  //set the idt entry with corresponding handler
	idt[13] = e13;

	idt_desc_t e14 = generic_interrupt;
	SET_IDT_ENTRY(e14, exception_14);  //set the idt entry with corresponding handler
	idt[14] = e14;

	idt_desc_t e15 = generic_interrupt;
	SET_IDT_ENTRY(e15, exception_15);  //set the idt entry with corresponding handler
	idt[15] = e15;

	idt_desc_t e16 = generic_interrupt;
	SET_IDT_ENTRY(e16, exception_16);  //set the idt entry with corresponding handler
	idt[16] = e16;

	idt_desc_t e17 = generic_interrupt;
	SET_IDT_ENTRY(e17, exception_17);  //set the idt entry with corresponding handler
	idt[17] = e17;

	idt_desc_t e18 = generic_interrupt;
	SET_IDT_ENTRY(e18, exception_18);  //set the idt entry with corresponding handler
	idt[18] = e18;

	idt_desc_t e19 = generic_interrupt;
	SET_IDT_ENTRY(e19, exception_19);  //set the idt entry with corresponding handler
	idt[19] = e19;

	idt_desc_t e20 = generic_interrupt;
	SET_IDT_ENTRY(e20, exception_20);  //set the idt entry with corresponding handler
	idt[20] = e20;

	idt_desc_t e21 = generic_interrupt;
	SET_IDT_ENTRY(e21, exception_21);  //set the idt entry with corresponding handler
	idt[21] = e21;

	idt_desc_t e22 = generic_interrupt;
	SET_IDT_ENTRY(e22, exception_22);  //set the idt entry with corresponding handler
	idt[22] = e22;

	idt_desc_t e23 = generic_interrupt;
	SET_IDT_ENTRY(e23, exception_23);  //set the idt entry with corresponding handler
	idt[23] = e23;

	idt_desc_t e24 = generic_interrupt;
	SET_IDT_ENTRY(e24, exception_24);  //set the idt entry with corresponding handler
	idt[24] = e24;

	idt_desc_t e25 = generic_interrupt;
	SET_IDT_ENTRY(e25, exception_25);  //set the idt entry with corresponding handler
	idt[25] = e25;

	idt_desc_t e26 = generic_interrupt;
	SET_IDT_ENTRY(e26, exception_26);  //set the idt entry with corresponding handler
	idt[26] = e26;

	idt_desc_t e27 = generic_interrupt;
	SET_IDT_ENTRY(e27, exception_27);  //set the idt entry with corresponding handler
	idt[27] = e27;

	idt_desc_t e28 = generic_interrupt;
	SET_IDT_ENTRY(e28, exception_28);  //set the idt entry with corresponding handler
	idt[28] = e28;

	idt_desc_t e29 = generic_interrupt;
	SET_IDT_ENTRY(e29, exception_29);  //set the idt entry with corresponding handler
	idt[29] = e29;

	idt_desc_t e30 = generic_interrupt;
	SET_IDT_ENTRY(e30, exception_30);  //set the idt entry with corresponding handler
	idt[30] = e30;

	idt_desc_t e31 = generic_interrupt;
	SET_IDT_ENTRY(e31, exception_31);  //set the idt entry with corresponding handler
	idt[31] = e31;

	idt_desc_t sys80 = generic_interrupt;  //FOR SYSTEM CALLS 
	sys80.reserved3 = 1;                   //trap gate
	sys80.dpl = 3;
	SET_IDT_ENTRY(sys80, system_call_handler);  //set the idt entry with corresponding handler;
	idt[SYSCALL_ENTRY] = sys80;

	idt_desc_t rtc40 = generic_interrupt;   //FOR RTC HANDLER
	SET_IDT_ENTRY(rtc40, rtc_int);
	idt[RTC_ENTRY] = rtc40;

	idt_desc_t keyboard = generic_interrupt;  //FOR KEYBOARD HANDLER 
	SET_IDT_ENTRY(keyboard, keyboard_int);
	idt[KEYB_ENTRY] = keyboard;

	idt_desc_t mouse = generic_interrupt; //FOR MOUSE HANDLER
	SET_IDT_ENTRY(mouse, mouse_int);
	idt[MOUSE_ENTRY] = mouse;

	idt_desc_t pit = generic_interrupt; //FOR PIT HANDLER
	SET_IDT_ENTRY(pit,pit_int);
	idt[0x20] = pit;


	return 0;

}

/*  
*   exception_0()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 0 
*/
 void exception_0()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Divide by zero error :(\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_1()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 1 
*/
 void exception_1()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Debug Exception :(\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_2()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 2 
*/
 void exception_2()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Non Maskable Interrupt!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_3()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 3 
*/
 void exception_3()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Breakpoint!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_4()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 4 
*/
 void exception_4()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Overflow detected!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_5()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 5 
*/
 void exception_5()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Bound Range exceeded",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_6()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 6 
*/
 void exception_6()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Invalid Opcode!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_7()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 7 
*/
 void exception_7()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Device/ Co-processor not available!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_8()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 8 
*/
 void exception_8()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Double Fault!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_9()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 9
*/
 void exception_9()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Co-processor segment overrun",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_10()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 10 
*/
 void exception_10()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Invalid Task State Segment",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_11()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 11 
*/
 void exception_11()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Segment not present!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_12()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 12 
*/
 void exception_12()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Stack Exception!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_13()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 13 
*/
 void exception_13()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"General Protection!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_14()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 14 
*/
 void exception_14(int32_t esp)
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Page Fault!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_15()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 15 
*/
 void exception_15()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Intel Reserved!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_16()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 16 
*/
 void exception_16()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Floating Point Error!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_17()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 17 
*/
 void exception_17()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Alignment Check!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_18()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 18
*/
 void exception_18()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Machine Check!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_19()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 0 
*/
 void exception_19()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"SIMD Floating Point!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_20()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 0 
*/
 void exception_20()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Virtualisation Exception!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_21()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 0 
*/
 void exception_21()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Control Protection Exception!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_22()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 22 
*/
 void exception_22()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E22!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_23()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 23 
*/
 void exception_23()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E23!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_24()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 24 
*/
 void exception_24()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E24!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_25()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 25 
*/
void exception_25()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E25!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_26()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 26
*/
 void exception_26()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E26!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_27()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 27 
*/
 void exception_27()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E27!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_28()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 28
*/
 void exception_28()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E28!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_29()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 29 
*/
 void exception_29()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E29!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_30()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 30 
*/
 void exception_30()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E30!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}

/*  
*   exception_31()
*   Inputs: NONE
*   Return Value: NONE 
*	Function: Exception handler for idt entry 31 
*/
 void exception_31()
{
	clear(); //clear the whole screen and print the expetion string!
	write(1,"Reserved E31!\n",BUF_SIZE_EXCEPTION);
	status_bar();
	halt(0);
}



