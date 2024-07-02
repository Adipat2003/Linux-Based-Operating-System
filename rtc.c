#include "rtc.h"
#include "filesys_driver.h"
//interrupt flag for read function
// volatile int rtc_int_flag;

/* RTC Initializer
 * 
 * Initializes the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void rtc_init(){
    cli();

    char prev;
    enable_irq(RTC_IRQ);
    /* Algorithm from osdev/rtc webpage */
    outb(STATUS_REG_B, RTC_REG_PORT);
    prev = inb(RTC_RW_PORT);
    outb(STATUS_REG_B, RTC_REG_PORT);
    outb(prev | 0x40, RTC_RW_PORT); // 0x40 to turn off bit 6 of register B

    int i;
    for(i = 0; i < 3; i++){
        terminals[i].rtc_flag = 1;
        terminals[i].rtc_counter = 0;
        terminals[i].rtc_counter_reset = 0;
    }

    sti();
    return;
}

/* RTC Handler
 * 
 * Handles the RTC interrupt
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void rtc_handler(){
    cli();
    
    //test_interrupts();

    /* Algorithm from osdev/rtc webpage */
    outb(0x0C, RTC_REG_PORT);   // 0x0C to select register C
    inb(RTC_RW_PORT);

    // rtc_int_flag = 0;   // interrupt active, so set flag

    int i;
    for(i = 0; i < 3; i++){
        if(terminals[i].rtc_counter == 0){
            continue;
        }
        terminals[i].rtc_counter--;
        if(terminals[i].rtc_counter == 0){
            terminals[i].rtc_flag = 0;
            terminals[i].rtc_counter = terminals[i].rtc_counter_reset;
        }
    }

    // printf("Testing RTC");
    send_eoi(RTC_IRQ);
    sti();
    return;
}

//need open, read, write, close functions to demonstrate that we can change clock frequency
//rtc open should set frequency to 2Hz
//rtc read must return only after an rtc interrupt has occurred (use a flag)
//rtc write must get input paramter through a buffer (don't ready value directly)
//there is a one line formula to check if a number is a power of two (think binary or in terms of bits) (frequencies must be a power of 2)
//test cases should demonstrate that we are able to change rtc through all frequencies

/* RTC Open
 * 
 * Sets the initial RTC frequency to 2Hz
 * Inputs: File desciptor
 * Outputs: None
 * Side Effects: None
 */
int32_t rtc_open(const uint8_t* filename){
    
    //32768Hz = 1000000000000000b
    //2Hz = 10b
    //frequency = frequency >> (rate - 1)
    //to make frequency 2Hz, need to set rate to 15

    terminals[active_terminal].rtc_counter = 1024/2;
    terminals[active_terminal].rtc_counter_reset = 1024/2;

    // int rate = 15;  // set frequency to 2Hz
    int rate = 6;   // set frequency to 1024Hz [32768 >> (6 - 1) = 1024Hz]
    rate &= 0x0F;			// rate must be above 2 and not over 15
    cli();

    /* Algorithm from osdev/rtc webpage */	
    outb(STATUS_REG_A, RTC_REG_PORT);		
    char prev = inb(RTC_RW_PORT);	
    outb(STATUS_REG_A, RTC_REG_PORT);		
    outb((prev & 0xF0) | rate, RTC_RW_PORT); // write only our rate to register A--rate is bottom 4 bits

    sti();

    // int fdValue = fdIndex; //We get what the current index is for fd, so we know which entry it needs to go into for the array
    // if(fdValue < 2 || fdValue > 7){
    //     return -1; //We can not open file because not enough space in file descriptor table
    // }
    
    return 0;
}

/* RTC Read
 * 
 * Waits until an RTC interrupt occurs
 * Inputs: File desciptor
 *         Buf
 *         nbytes
 * Outputs: None
 * Side Effects: None
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    // rtc_int_flag = 1;   // set interrupt flag

    while(terminals[active_terminal].rtc_flag);    // wait for interrupt to occur

    terminals[active_terminal].rtc_flag = 1;

    return 0;
}

/* RTC Write
 * 
 * Sets RTC to the requested frequency if frequency is valid
 * Inputs: File desciptor
 *         Buf: Buffer containing requested frequency
 *         nbytes: Size of frequency requested (must be 4 bytes)
 * Outputs: None
 * Side Effects: None
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL || nbytes != 4){ // check if buffer is NULL or number of bytes is not 4
        return -1;
    }

    uint32_t* new_freq_temp = (uint32_t*)buf;
    uint32_t new_freq = *new_freq_temp; // get the requested frequency

    if(new_freq < 2 || new_freq > 1024){    //check if frequency is within valid range
        return -1;
    }

    //check if frequency is a power of 2
    if ((new_freq & (new_freq - 1)) != 0){
        return -1;
    }

    terminals[active_terminal].rtc_counter = 1024/new_freq;
    terminals[active_terminal].rtc_counter_reset = 1024/new_freq;

    // int rate = 0;
    int rate = 6;   // set frequency to 1024Hz [32768 >> (6 - 1) = 1024Hz]
    // while(new_freq != 32768){
    //     rate++; //get rate (number to shift by)
    //     new_freq = new_freq << 1;
    // }
    // rate++; //increment again to account for rate - 1 in formula
    rate &= 0x0F;			//rate must be above 2 and not over 15
    //now rate is the correct value for the input frequency

    cli();

    /* Algorithm from osdev/rtc webpage */	
    outb(STATUS_REG_A, RTC_REG_PORT);		
    char prev = inb(RTC_RW_PORT);	
    outb(STATUS_REG_A, RTC_REG_PORT);		
    outb((prev & 0xF0) | rate, RTC_RW_PORT); //write only our rate to register A--rate is bottom 4 bits

    sti(); 

    return 0;
}

/* RTC Close
 * 
 * Does nothing
 * Inputs: File desciptor
 * Outputs: None
 * Side Effects: None
 */
int32_t rtc_close(int32_t fd){
    terminals[active_terminal].rtc_counter = 0;
    terminals[active_terminal].rtc_counter_reset = 0;
    return 0;   // do nothing
}
