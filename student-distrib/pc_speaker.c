/* PC SPEAKER */
#include "pc_speaker.h"

////// OSDEV code was copied, pasted and modified here!!////////
void play_sound(uint32_t freq) 
{
	
 	uint32_t divv;
 	uint8_t temp;
 
    //Set the PIT to the desired frequency

	 divv = 1193180 / freq;


 	outb(0xB6, 0x43);
 	outb((uint8_t)(divv), 0x42);
 	outb((uint8_t)(divv >> 8), 0x42);
 
    //And play the sound using the PC speaker
 	temp = inb(0x61);
  	if (temp != (temp | 3)) 
  		outb((temp | 3), 0x61);
  	
}
 

//stop sound 
void stop_music() 
{
	int i;
 	uint8_t temp = (inb(0x61) & 0xFC);
  	outb(temp, 0x61);
  	for(i = 0; i < 900000; i++)
	{

	}
}

void pattern()
{
	int i;
	for(i = 0; i < 100000; i++)
		play_sound(G3);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(E3);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(D3);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(B3);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(G4);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(E4);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(D4);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(B4);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(G5);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(E5);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(D6);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(B6);
	stop_music();

	for(i = 0; i < 100000; i++)
		play_sound(G7);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(E7);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(D7);
	stop_music();
	for(i = 0; i < 100000; i++)
		play_sound(B7);
	stop_music();
	return;
}

void jingle()
{
	int a,z;
	 for(a = 0; a < 7; a++)
	 {
	 	for(z = 0; z < 200000; z++)
	 		play_sound(E3);
	 	stop_music();
	 }

	 for(z = 0; z < 180000; z++)
	 	play_sound(G3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(C3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(E3);
	 	stop_music();	
	 for(a = 0; a < 5; a++)
	 {
	 	for(z = 0; z < 180000; z++)
	 		play_sound(F3);
	 	stop_music();
	 }

	 for(a = 0; a < 4; a++)
	 {
	 	for(z = 0; z < 180000; z++)
	 		play_sound(E3);
	 	stop_music();
	 }
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 	stop_music();	
	 for(z = 0; z < 180000; z++)
	 	play_sound(E3);
	 	stop_music();	
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 	stop_music();	
	 for(z = 0; z < 180000; z++)
	 	play_sound(G3);
	 	stop_music();		
	 for(a = 0; a < 7; a++)
	 {
	 	for(z = 0; z < 180000; z++)
	 		play_sound(E3);
	 	stop_music();
	 }
	 for(z = 0; z < 180000; z++)
	 	play_sound(G3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(C3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 	stop_music();		
	 for(z = 0; z < 180000; z++)
	 	play_sound(E3);
	 stop_music();
	 for(a = 0; a < 5; a++)
	 {
	 	for(z = 0; z < 180000; z++)
	 		play_sound(F3);
	 	stop_music();
	 }
	 for(a = 0; a < 3; a++)
	 {
	 	for(z = 0; z < 180000; z++)
	 		play_sound(E3);
	 	stop_music();
	 }
	 for(z = 0; z < 180000; z++)
	 	play_sound(G3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(G3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(F3);
	 stop_music();
	 for(z = 0; z < 180000; z++)
	 	play_sound(D3);
	 	stop_music();	
	 for(z = 0; z < 180000; z++)
	 	play_sound(C3);
	 stop_music();

	 stop_music();

}

