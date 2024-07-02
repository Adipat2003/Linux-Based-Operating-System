#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"

char scan_code1[SCAN_CODE_SIZE] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
                    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
                    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
                    'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' ', '\0'}; /* Scan code 1 from os dev for key presses only */

char scan_code1_shift[SCAN_CODE_SIZE] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
                    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
                    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
                    'B', 'N', 'M', '<', '>', '?', '\0', '\0', '\0', ' ', '\0'}; /* Scan code 1 from os dev for key presses only */

char scan_code1_capslock[SCAN_CODE_SIZE] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
                    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', '\0', 'A', 'S',
                    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
                    'B', 'N', 'M', ',', '.', '/', '\0', '\0', '\0', ' ', '\0'}; /* Scan code 1 from os dev for key presses only */

char scan_code1_capslock_shift[SCAN_CODE_SIZE] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
                    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', '\0', 'a', 's',
                    'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', '\0', '|', 'z', 'x', 'c', 'v',
                    'b', 'n', 'm', '<', '>', '?', '\0', '\0', '\0', ' ', '\0'}; /* Scan code 1 from os dev for key presses only */

int shift_held = 0; //shift flag
int ctrl_held = 0;  //CTRL flag
int alt_held = 0;   //ALT flag
int capslock_on = 0;    //CAPS flag

int32_t active_terminal = 0;

/* Keyboard Initializer
 * 
 * Initializes the Keyboard
 * Inputs: None
 * Outputs: None
 * Side Effects: Connects keyboard to the primary PIC
 */
void kb_init(){
    cli();

    enable_irq(KEYBOARD_IRQ);   /* Hooking KB up to PIC */

    sti();
    return;
}

/* Keyboard Handler
 * 
 * Handles the Keyboard interrupt
 * CP2: Adds appropriate input characters to the keyboard buffer and prints them to the line
 * Inputs: None
 * Outputs: None
 * Side Effects: Displays characters to screen and adds characters into terminal buffer
 */
void kb_handler(){
    cli();
    
    update_vidmap(active_terminal);

    int input_char = inb(KEYBOARD_PORT);
    /* Setting flags for keyboard to determine which scan_code set to print from */
    if(input_char == LSHIFT_PRESS || input_char == RSHIFT_PRESS)
        shift_held = 1;
    if(input_char == LSHIFT_RELEASE || input_char == RSHIFT_RELEASE)
        shift_held = 0;
    if(input_char == CAPSLOCK_PRESS)
        capslock_on ^= 1;
    if(input_char == CTRL_PRESS)
        ctrl_held = 1;
    if(input_char == CTRL_RELEASE)
        ctrl_held = 0;
    if(input_char == ALT_PRESS)
        alt_held = 1;
    if(input_char == ALT_RELEASE)
        alt_held = 0;

    if(ctrl_held && scan_code1[input_char] == 'l')
        terminal_clear();   //clear screen if CTRL+l/L

    if(alt_held && input_char == F1_PRESS){
        if (switch_terminal(0) == -1){
            terminal_clear();
            printf("Switching not possible");
        }
    }
    
    if(alt_held && input_char == F2_PRESS){
        if (switch_terminal(1) == -1){
            terminal_clear();
            printf("Switching not possible");
        }
    }
    
    if(alt_held && input_char == F3_PRESS){
        if (switch_terminal(2) == -1){
            terminal_clear();
            printf("Switching not possible");
        }
    }

    if(scan_code1[input_char] == '\0' || scan_code1[input_char] == '\t'){   //return if invalid input
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }
    
    /* Ensuring input to print to screen is within keypress range and isnt a NULL byte */
    if(input_char == ENTER_PRESS){
        putc(scan_code1_capslock_shift[input_char]);
        terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = '\n';  //enter pressed, so end of input--add newline
        terminals[active_terminal].kb_buf_chars++;
        terminals[active_terminal].read_flag = 1;  //set enter flag
    }else if(input_char == BSPACE_PRESS){
        if(terminals[active_terminal].kb_buf_chars != 0){
            putc(scan_code1[input_char]);
            terminals[active_terminal].kb_buf_chars--;    //delete a character
            if(terminals[active_terminal].kb_buf_chars == 0){
                terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = '\0';  //set deleted character in buffer to NULL
            } 
        }
    }else{
        if(terminals[active_terminal].kb_buf_chars < KB_BUF_SIZE - 1){    //check if characters should still be accepted
            if((input_char >= SCAN_CODE_MIN) && (input_char <= SCAN_CODE_MAX) && (!ctrl_held) && (!alt_held)){    /* Printing char to console if KB press was in range of basic chars */
                if(capslock_on && shift_held){  //check if capslock and shift are active
                    putc(scan_code1_capslock_shift[input_char]);
                    terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = scan_code1_capslock_shift[input_char];
                    terminals[active_terminal].kb_buf_chars++;
                }else if(shift_held && !capslock_on){   //check if shift is active and caps is not
                    putc(scan_code1_shift[input_char]);
                    terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = scan_code1_shift[input_char];
                    terminals[active_terminal].kb_buf_chars++;
                }else if(capslock_on){  //check if caps is active
                    putc(scan_code1_capslock[input_char]);
                    terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = scan_code1_capslock[input_char];
                    terminals[active_terminal].kb_buf_chars++;
                }else{
                    putc(scan_code1[input_char]);   //print all other characters
                    terminals[active_terminal].kb_buf[terminals[active_terminal].kb_buf_chars] = scan_code1[input_char];
                    terminals[active_terminal].kb_buf_chars++;
                }
            }
        }
    }
    
    update_vidmap(curr_ter);
    send_eoi(KEYBOARD_IRQ); 
    sti();
    return;
}

