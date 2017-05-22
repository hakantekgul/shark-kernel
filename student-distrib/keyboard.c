#include "keyboard.h"
#include "terminal.h"
#include "rtc.h"
#include "filesystem.h"
#include "pc_speaker.h"


int CAPS_LOCK = 0;
int SHIFT = 0;
int CTRL = 0;
int ALT = 0;
int prompt_size = 7;


/*
*   keyboard_init(void)
*   Inputs: NONE
*   Return Value: NONE
*   Function: initializes keyboard by enabling IRQ1 
*/
void keyboard_init(void)
{
    enable_irq(1);
    return;
}



/*
*   keyboard_handler1(void)
*   Inputs: NONE
*   Return Value: NONE
*   Function: calls a helper to do handler, this is used to reduce code and make cleaner 
*/
void keyboard_handler1(void){
    keyboard_handler();
    update_cursor(whole_screen_buffer_index);
    if(whole_screen_buffer_index >= WHOLE_SCREEN_BUFFER_SIZE){
       vertical_scroll();
    }
    send_eoi(1);
}


/* Maintain a global location for the current video memory to write to */
//static int current_loc = 0;
// Video memory starts at 0xb8000. Make it a constant pointer to
//   characters as this can improve compiler optimization since it
//   is a hint that the value of the pointer won't change 
//static char *const vidptr = (char*)0xb8000;
/*
*   keyboard_handler()
*   Inputs: NONE
*   Return Value: NONE
*   Function: interrupt handler for RTC. See comments below to get interrupt again 
*/
void keyboard_handler(void)
{
    //Clear other interrupts before starting handler
    int i,prev_count,temp1;
    unsigned char status;
    unsigned char keycode;
    unsigned char blank[1];
    blank[0] = 0;

    /* Acknowledgment */
    status = inb(KEYBOARD_IO_PORT);
    /* Lowest bit of status will be set if buffer is not empty */
    if (status & 0x01) {
        keycode = inb(DATA_PORT);
        /* Only print characters on keydown event that have
         * a non-zero mapping */
        if(kboard_map[keycode])
        {
            //this is a charcater that we support
            //first we check for all the non printable characters and handle

            if(buffer_index >= BUFFER_SIZE - 2)
            { //-2 since we always must allow '\n'
                //buffer is full, so do not take in any more chars, but still write to screen

                if(keycode == BACKSPACE_KEY)
                {
                    do_backspace();
                    return;
                }

                if(keycode == ENTER_KEY)
                {

                    enter_flag = 1;
                    buffer[BUFFER_SIZE - 1] = '\n';
                    buffer_index++;
                    whole_screen_buffer_index = (whole_screen_buffer_index + TERMINAL_WIDTH) - (whole_screen_buffer_index % TERMINAL_WIDTH);
                    return;
                }
                //stop reading things to the buffer (do nothing)
                else
                {
                    return;
                }
            }

            //if enter is pressed
            if(keycode == ENTER_KEY)
            {
                enter_flag = 1;
                whole_screen_buffer_index = (whole_screen_buffer_index + TERMINAL_WIDTH) - (whole_screen_buffer_index % TERMINAL_WIDTH);
                buffer[buffer_index + 1] = '\n';
                buffer_index++;
                return;
            }
            //backspace
            if(keycode == BACKSPACE_KEY){
                
                    do_backspace();
                return;
            }

            //if we press caps lock
            if(keycode == CAPS_LOCK_KEY){
                if(CAPS_LOCK == 1){
                    CAPS_LOCK = 0;
                }
                else{
                    CAPS_LOCK = 1;
                }
                return;
            }

            //press shift
            else if(keycode == SHIFT1_KEY || keycode == SHIFT2_KEY){
                SHIFT = 1;
                return;
            }
            //release shift
            else if(keycode == RELEASE_SHIFT1_KEY || keycode == RELEASE_SHIFT2_KEY || keycode == RELEASE_SHIFT3_KEY || keycode == RELEASE_SHIFT4_KEY){
                SHIFT = 0;
                return;
            }
            //press ctrl
            else if(keycode == CTRL_KEY){
                CTRL = 1;
                return;
            }
            //relase ctrl
            else if(keycode == RELEASE_CTRL_KEY){
                CTRL = 0;
                return;
            }
            //press alt
            else if(keycode == ALT_KEY){
                ALT = 1;
                return;
            }
            //release alt
            else if(keycode == RELEASE_ALT_KEY){
                ALT = 0;
                return;
            }

            //at this point it is a regular character that we do something with
            if(CTRL == 1){
                switch(keycode){
                    case L_KEY:    //ctrl - l
                        clear_screen();
                        terminal_write(1, "391OS> ", prompt_size);
                        break;
                    case CTRL1_KEY:     //ctrl - 1
                        //read_directory(0,0,0);
                        break;
                    case CTRL2_KEY:     //ctrl - 2
                        read_file_by_name_test();
                        break;
                    case CTRL3_KEY:     //ctrl - 3
                        read_by_index_test();
                        break;
                    case CTRL4_KEY:     //ctrl - 4
                        clear_screen();
                        rtc_test();
                        break;
                    case CTRL5_KEY:     //ctrl - 5
                        jingle();
                        break;
                    default:
                        break;

                }
                return;
            }

            //up key
            if(keycode == UP_KEY){
                temp1 = buffer_index + 1;
                for(i = 0; i < temp1; i++){
                    buffer[i] = 0;
                    whole_screen_buffer[whole_screen_buffer_index - i] = 0;
                    whole_screen_buffer_index --;
                    terminal_write(1, blank, 1);
                    update_cursor(whole_screen_buffer_index);
                    whole_screen_buffer_index --;
                }
                buffer_index = -1;
                if(buffer_index >= prev_command_index - 1){
                    return;
                }
                for(i = 0; i < prev_command_index; i++){
                    buffer[buffer_index + i + 1] = prev_command[i];
                }
                prev_count = buffer_index;
                terminal_write(1, buffer, prev_command_index);
                buffer_index = prev_count + prev_command_index;
                return;
            }
            //down key
            if(keycode == DOWN_KEY){
                temp1 = buffer_index + 1;
                for(i = 0; i < temp1; i++){
                    do_backspace();
                }
                return;
            }
            //we print this character now
            //CAPS lock is on
            if(CAPS_LOCK == 1){
                if(SHIFT == 1){
                    //check if the key is a letter, if so, print lowercase
                    if((keycode >= LETTER_INDEX1 && keycode <= LETTER_INDEX2) || (keycode >= LETTER_INDEX3 && keycode <= LETTER_INDEX4) || (keycode >= LETTER_INDEX5 && keycode <= LETTER_INDEX6)){
                        putc_terminal(kboard_map[keycode], whole_screen_buffer_index % TERMINAL_WIDTH, whole_screen_buffer_index / TERMINAL_WIDTH);
                        buffer[buffer_index + 1] = kboard_map[keycode];
                        buffer_index++;
                        whole_screen_buffer[whole_screen_buffer_index] = kboard_map[keycode];        
                    } 
                    else{
                        putc_terminal(kboard_map_shift[keycode], whole_screen_buffer_index % TERMINAL_WIDTH, whole_screen_buffer_index / TERMINAL_WIDTH);
                        buffer[buffer_index + 1] = kboard_map_shift[keycode];
                        buffer_index++;
                        whole_screen_buffer[whole_screen_buffer_index] = kboard_map_shift[keycode];
                    }
                }
                else{
                    putc_terminal(kboard_map_caps[keycode], whole_screen_buffer_index % TERMINAL_WIDTH, whole_screen_buffer_index / TERMINAL_WIDTH);
                    buffer[buffer_index + 1] = kboard_map_caps[keycode];
                    buffer_index++;
                    whole_screen_buffer[whole_screen_buffer_index] = kboard_map_caps[keycode];
                }
                whole_screen_buffer_index++;
                return;
            }

            //shift is on
            if(SHIFT == 1){
                putc_terminal(kboard_map_shift[keycode], whole_screen_buffer_index % TERMINAL_WIDTH, whole_screen_buffer_index / TERMINAL_WIDTH);
                buffer[buffer_index + 1] = kboard_map_shift[keycode];
                buffer_index++;
                whole_screen_buffer[whole_screen_buffer_index] = kboard_map_shift[keycode];
                whole_screen_buffer_index++;
                return;
            }

            //alt is on -- for terminal swapping
            if(ALT == 1){
                if(keycode == F1_KEY){
                    cli();
                    status_bar();
                    before_swap(1);
                }
                else if(keycode == F2_KEY){
                    cli();
                    status_bar();
                    before_swap(2);
                }
                else if(keycode == F3_KEY){
                    cli();
                    status_bar();
                    before_swap(3);
                }
                return;
            }

            //at this point, just print regular character.
            putc_terminal(kboard_map[keycode], whole_screen_buffer_index % TERMINAL_WIDTH, whole_screen_buffer_index / TERMINAL_WIDTH);
            buffer[buffer_index + 1] = kboard_map[keycode];
            buffer_index++;
            whole_screen_buffer[whole_screen_buffer_index] = kboard_map[keycode];
            whole_screen_buffer_index++;
            return;   
        }
    }
    return;


}


