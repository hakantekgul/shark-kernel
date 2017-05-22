/* sound.c */

/**** SOUND BLASTER 16 DRIVER ***/
#include "sound.h"

void init_sound()
{
	reset_sb();
	return;
}
void reset_sb()
{
	int i,x;
	outb(1,DSP_RESET); //write 1 to reset port

	//wait for 3 microseconds
	for(i = 0; i < 3000; i++)
	{
		x = 0;
		x = x * i * x * (x + 1);
	}

	outb(0,DSP_RESET); //write 0 to reset port

	while((inb(BASE+DSP_RD_BUF) & BIT_SEVEN) == 0) //poll until buffer is set
	{

	}

	while((inb(DSP_READ+BASE) != RESET_CHECK)) //poll until xAA is received
	{

	}
}

void write_sb(uint8_t w)
{
	while((inb(DSP_WRITE+BASE) & BIT_SEVEN) == 1)
	{

	}

	outb(w,DSP_WRITE);
}

uint8_t read_sb()
{
	while((inb(BASE+DSP_RD_BUF) & BIT_SEVEN) == 0)
	{

	}

	uint8_t read;
	read = inb(DSP_READ);

	return read;
}


void set_rate(uint16_t freq)
{
	write_sb(SAMPLING); //tell the device that you are writing rate
	write_sb(((freq)>>SHIFTING) & MASK_EIGHT); //high byte
	write_sb((freq) & MASK_EIGHT); //low byte

}

int init_dma_transfer()
{
	return 0;
}


