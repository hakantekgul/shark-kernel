/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"


/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
*   i8259_init(void)
*   Inputs: NONE
*   Return Value: NONE
*	Function: initializes PIC by sending ICWs to both slave and mask pics  
*/
void
i8259_init(void)
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////

	// initalize MASTER PIC first
	outb(ICW1, MASTER_8259_PORT);   //send ICW1 to port1            
	outb(ICW2_MASTER, PIC1_DATA);   //send ICW2 
	outb(ICW3_MASTER, PIC1_DATA);   //send ICW3 
	outb(ICW4, PIC1_DATA);          //send ICW4

	// initialize SLAVE PIC 
	outb(ICW1, SLAVE_8259_PORT);    //send ICW1 to port1
	outb(ICW2_SLAVE, PIC2_DATA);    //send ICW2
	outb(ICW3_SLAVE, PIC2_DATA);    //send ICW3
	outb(ICW4, PIC2_DATA);          //send ICW4 

	slave_mask = 0xFF; //mask all the bits in slave in init all 1s
	master_mask = 0xFF; //mask all the bits in master in init all 1s

	enable_irq(SLAVE_PIC_IRQ); //enable slave PIC  

	return;
}

/* Enable (unmask) the specified IRQ */
/*
*   enable_irq(uint32_t irq_num)
*   Inputs: interrupt request line number 0 - 15 - unsigned 
*   Return Value: NONE
*	Function: unmasks the specified IRQ - set it to 0 so that PIC will not ignore it. See comment below  
*/
void
enable_irq(uint32_t irq_num)
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	// each bit in the 8 bit slave mask and master mask corresponds to irq
	// When a bit is set, the PIC ignores the request and continues normal operation. 
	// Note that setting the mask on a higher request line will not affect a lower line. 
	// Masking IRQ2 will cause the Slave PIC to stop raising IRQs.

	
	int slave_irq; //irq number on slave PIC
	int masking = 0x7F; //01111111 --> we are masking out irq and we want to shift 0 
	int i; //loop index 

	if(irq_num < MASTER_MAX) //master PIC
	{
		for(i = 0; i < (MASTER_MAX-1-irq_num); i++)
		{
			masking = (masking >> 1) + SHIFT_FIX; //shift 0 irq times and 1 so that other irq remain same
		}
		master_mask = master_mask & masking; //and it to set the master mask 
		outb(master_mask, PIC1_DATA); //enable the specified irq 

	}

	else if(irq_num > MASTER_MAX - 1) //slave PIC
	{
		slave_irq = irq_num - MASTER_MAX; //get the slave irq 
		for(i = 0; i < (MASTER_MAX-1-slave_irq); i++)
		{
			masking = (masking >> 1) + SHIFT_FIX; //shift 0 irq times and 1 so that other irq remain same
		}
		slave_mask = slave_mask & masking; //and it to set slave mask
		outb(slave_mask, PIC2_DATA); //enable the specified irq on slave 
	}
	
	
}

/* Disable (mask) the specified IRQ */
/*
*   disable_irq(uint32_t irq_num)
*   Inputs: interrupt request line number 0 - 15 - unsigned 
*   Return Value: NONE
*	Function: masks the specified IRQ - set it to 1 so that PIC will ignore it. See comment below  
*/
void
disable_irq(uint32_t irq_num)
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	// each bit in the 8 bit slave mask and master mask corresponds to irq
	// When a bit is set, the PIC ignores the request and continues normal operation. 
	// Note that setting the mask on a higher request line will not affect a lower line. 
	// Masking IRQ2 will cause the Slave PIC to stop raising IRQs.


	int slave_irq; //irq number on slave PIC
	uint8_t masking = 0x80; //1000000 --> we want to unmask the irq and want to shift 1 
	int i;


	if(irq_num < MASTER_MAX) //master PIC
	{
		for(i = 0; i < (MASTER_MAX-1-irq_num); i++)
		{
			masking = (masking >> 1); //shift 1 irq times
		}
		master_mask = master_mask | masking; //OR it to disable
		outb(master_mask, PIC1_DATA);
	}
	else if(irq_num > MASTER_MAX - 1) //slave PIC
	{
		slave_irq = irq_num - MASTER_MAX;
		for(i = 0; i < (MASTER_MAX-1-slave_irq); i++)
		{
			masking = (masking >> 1);  //shift 1 irq times 
		}
		slave_mask = slave_mask | masking; //OR it to disable 
		outb(slave_mask, PIC2_DATA);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
/*  
*   send_eoi(uint32_t irq_num)
*   Inputs: interrupt request line number 0 - 15 - unsigned 
*   Return Value: NONE
*	Function: Sends an end of interrupt signal to the PIC. 
*/
void
send_eoi(uint32_t irq_num)
{
	///////////////////////////////////// NOTE: OSDEV CODE WAS COPIED AND PASTED AND MODIFIED HERE! ////////////////////////////////
	if(irq_num > MASTER_MAX-1)
	{	//from slave
		outb(EOI | (irq_num - MASTER_MAX), PIC2_COMMAND); //end of interrupt...
		outb(EOI | SLAVE_PIC_IRQ, MASTER_8259_PORT); //send eoi to master too! 0x02 stands for second IRQ on master, which is connected to slave 
		return;
	}
	outb(EOI | irq_num, PIC1_COMMAND); //end of interrupt for master 
}

