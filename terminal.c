#include "terminal.h"
#include "lib.h"
//terminal screen is only 80 chars so write to second line +48 to account for all 128bytes

//note: keyboard driver should call terminal driver to add input to line buffer (should update the index as well)
//note: ctrl + l and ctrl + L should both clear screen
//but line buffer should not be cleared if this is executed midway through typing

terminal_t terminals[3];
int32_t active_terminal;
int32_t pidFlag;
int32_t initializeFlag;
int32_t previous_terminal;
// int status1 = 0;
int32_t status2 = 0;
int32_t status3 = 0;

int32_t terminial_init(){
    int i;
    int j;
    int PIDIndex;
    for(i = 0; i < 3; i++){
        terminals[i].pid_idx = 0;
        for(j = 0; j < 4; j++){
            terminals[i].pids[j] = -1;
            terminals[i].x_cursor = 0;
            terminals[i].y_cursor = 0;
            terminals[i].x_screen = 0;
            terminals[i].y_screen = 0;
        }
    }
    active_terminal = 0;
    
    terminals[0].video_mem = 0xb9000;
    terminals[1].video_mem = 0xba000;
    terminals[2].video_mem = 0xbb000;
    previous_terminal = 0;

    for(PIDIndex = 0; PIDIndex < 6; PIDIndex++){
        PIDArray[PIDIndex] = 0;
        isShell[PIDIndex] = 0;
    }
    
    return 0;
}

/* Terminal Open
 * 
 * Does nothing (returns 0)
 * Inputs: File Descriptor
 * Outputs: None
 * Side Effects: None
 */
int32_t terminal_open(const uint8_t* filename){
    //want to initialize buffer here? and buffer index var here? doesn't really matter tho
    //I would say set the buffer to all NULL here so the clearing is seamless in close()
    //where should the cursor be set initially? ctrl + L resets it but should it also start in a specific place when the terminal opens?
    int32_t i;
    for(i = 0; i < KB_BUF_SIZE; i++){   //set keyboard buffer to all NULL
        terminals[active_terminal].kb_buf[i] = NULL;
    }
    terminals[active_terminal].kb_buf_chars = 0;  //reset keyboard buffer index
    
    return 0;   //does nothing
}

/* Terminal Close
 * 
 * Resets the keyboard buffer to NULL
 * Inputs: File Descriptor
 * Outputs: None
 * Side Effects: None
 */
int32_t terminal_close(int32_t fd){
    //clear line buffer (set to null)?
    int32_t i;
    for(i = 0; i < KB_BUF_SIZE; i++){   //set keyboard buffer to all NULL
        terminals[active_terminal].kb_buf[i] = NULL;
    }
    terminals[active_terminal].kb_buf_chars = 0;  //reset keyboard buffer index

    return 0;
}

/* Terminal Read
 * 
 * Waits until Enter key is pressed, then copies keyboard buffer into argument
 * buffer
 * Inputs: File Descriptor
 *         Buf: Argument buffer to be copied into from Keyboard buffer
 *         nbytes: Number of bytes requested to be read
 * Outputs: None
 * Side Effects: None
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    //make sure to add new line character to end of buffer before returning. 128 CHAR LIMIT INCLUDES THE NEW LINE
    if(buf == NULL){    //check if buffer is invalid
        return -1;
    }
    
    int32_t i;
    int32_t bytes_read = 0;

    while(terminals[active_terminal].read_flag == 0);  //wait till Enter key is pressed

    cli();
    
    for(i = 0; i < nbytes; i++){    //copy keyboard buffer into argument buffer
        // if(terminal.kb_buf[i] == '\0')
        //     continue;
        ((char*)buf)[i] = terminals[active_terminal].kb_buf[i];
        bytes_read++;   //increment number of bytes read
    }

    //If we typed in exit, we need to remove the \n, so we can leave our shell if desired
    if(((char*)buf)[0] == 'e' && ((char*)buf)[1] == 'x' && ((char*)buf)[2] == 'i' && ((char*)buf)[3] == 't' && ((char*)buf)[4] == '\n'){
        ((char*)buf)[4] = '\0';
    }

    for(i = 0; i < terminals[active_terminal].kb_buf_chars; i++){ //set keyboard buffer to all NULL
        terminals[active_terminal].kb_buf[i] = NULL;
    }
    terminals[active_terminal].kb_buf_chars = 0;  //reset keyboard buffer index
    terminals[active_terminal].read_flag = 0;  //set Enter flag to 0
   
    sti();

    return bytes_read;
}

/* Terminal Write
 * 
 * Writes the argument buffer to the screen
 * Inputs: File Descriptor
 *         Buf: Argument buffer copied into from Keyboard buffer
 *         nbytes: Number of bytes requested to be written
 * Outputs: None
 * Side Effects: None
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    //also backspace on the new line should go back to previous line
    if(buf == NULL){    //check if buffer is invalid
        return -1;
    }
    
    int32_t i;
    int32_t bytes_wrote = 0;
    //int32_t wbytes = 0;

    if(nbytes <=0 /*|| ((char*)buf)[0] == '\0'*/)    //check if number of bytes requested is less than or equal to 0
        return -1;                             //and if argument buffer is empty

    cli();

    for(i = 0; i < nbytes; i++){    //print all non-NULL characters
        if(((char*)buf)[i] == '\0')
            continue;
        putc(((char*)buf)[i]);
        bytes_wrote++;
    }

    sti();

    return bytes_wrote;
}

/* Bad Read
 * 
 * Writes the argument buffer to the screen
 * Inputs: File Descriptor
 *         Buf: Argument buffer copied into from Keyboard buffer
 *         nbytes: Number of bytes requested to be written
 * Outputs: -1 for a bad call
 * Side Effects: Returns failure for stdin/stdout improper calls
 */
int32_t bad_read(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* Bad Write
 * 
 * Writes the argument buffer to the screen
 * Inputs: File Descriptor
 *         Buf: Argument buffer copied into from Keyboard buffer
 *         nbytes: Number of bytes requested to be written
 * Outputs: -1 for a bad call
 * Side Effects: Returns failure for stdin/stdout improper calls
 */
int32_t bad_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* Bad Write
 * 
 * Writes the argument buffer to the screen
 * Inputs: File Name
 * Outputs: -1 for a bad call
 * Side Effects: Returns failure for stdin/stdout improper calls
 */
int32_t bad_open(const uint8_t* filename){
    return -1;
}

/* Bad Close
 * 
 * Writes the argument buffer to the screen
 * Inputs: File Descriptor
 *  
 * Outputs: -1 for a bad call
 * Side Effects: Returns failure for stdin/stdout improper calls
 */
int32_t bad_close(int32_t fd){
    return -1;
}

int32_t switch_terminal (int32_t next_terminal){
    cli();

    change_col(next_terminal);
    
    if (currentPID > 5 || currentPID < 0)
        return -1;

    previous_terminal = active_terminal;

    if (previous_terminal == next_terminal)
        return -2;
    
    terminal_update(active_terminal);

    memcpy((void *)terminals[previous_terminal].video_mem, (uint8_t*)VIDEO, fourKB);

    if(next_terminal == 0 && flag0 == 1){
        flag0 = 0;
        active_terminal = next_terminal;
        send_eoi(1);
        // terminal_clear();
        // PCB_t temp = *get_PCB(terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1]);
        // asm("\t movl %%esp, %0" : "=r"(temp.current.exec_esp));
        // asm("\t movl %%ebp, %0" : "=r"(temp.current.exec_ebp));
        // execute((uint8_t*)"shell");
        // return 0;
    }

    if (next_terminal == 1 && status2 == 0){
        status2 = 1;
        flag1 = 0;
        active_terminal = next_terminal;
        send_eoi(1);
        // terminal_clear();
        // PCB_t temp = *get_PCB(terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1]);
        // asm("\t movl %%esp, %0" : "=r"(temp.current.exec_esp));
        // asm("\t movl %%ebp, %0" : "=r"(temp.current.exec_ebp));
        // execute((uint8_t*)"shell");
        // return
    }

    if (next_terminal == 2 && status3 == 0){
        status3 = 1;
        flag2 = 0;
        active_terminal = next_terminal;
        send_eoi(1);
        // terminal_clear();
        // PCB_t temp = *get_PCB(terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1]);
        // asm("\t movl %%esp, %0" : "=r"(temp.current.exec_esp));
        // asm("\t movl %%ebp, %0" : "=r"(temp.current.exec_ebp));
        // execute((uint8_t*)"shell");
        // return 0;
    }

    update_terminal_screen(next_terminal);
    //The if statements should not pass if e have already initalized terminals 2 and 3
    //If all if statements failed, set the active terminal to be the one you want to go to
    active_terminal = next_terminal;
    curr_ter = active_terminal;
    currentPID = terminals[active_terminal].pids[terminals[active_terminal].pid_idx - 1];

    page_table[VIDEO/FOUR_KB].present = 1;
    page_table[VIDEO/FOUR_KB].read_write = 1;
    page_table[VIDEO/FOUR_KB].user_supervisor = 1;
    page_table[VIDEO/FOUR_KB].addr_31_12 = VIDEO/FOUR_KB;

    flush_tlb();
    
    memcpy((uint8_t *) VIDEO, (void *) terminals[active_terminal].video_mem, fourKB);
    // need to do cursor code
    draw_cursor(terminals[active_terminal].x_cursor, terminals[active_terminal].y_cursor);
    sti();
    return 0;
}
