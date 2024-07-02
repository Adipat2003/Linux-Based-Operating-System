#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"
#include "rtc.h"
#include "filesys_driver.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* Divide by Zero Test
 * 
 * Asserts that the Divide by Zero exception is called when a Divide by Zero
 * scenario occurs
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Excpetion Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
int divide_zero_test(){
	TEST_HEADER;

	int a = 1;
	int b = 0;
	a = a/b;

	// printf("%d \n", 9/0);
	// int c = 9/0;
	return PASS;
}

/* Null Pointer Dereference Test
 * 
 * Asserts that the Paging Fault exception is called when a NULL pointer
 * is attempted to be dereferenced
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling, Paging
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h, paging.c/h,
 * paging_helper.h/S
 */
int deref_test(){
	TEST_HEADER;

	int *a = NULL;
	printf("%d \n", *a);
	//int b = *a;

	return PASS;
}

/* NMI Exception Test
 * 
 * Asserts that the NMI exception is called appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
inline int NMI_test(){
	TEST_HEADER;

	asm volatile("int $2");

	return PASS;

}

/* Overflow Exception Test
 * 
 * Asserts that the Overflow exception is called appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
inline int Overflow_test(){
	TEST_HEADER;

	asm volatile("int $4");

	return PASS;

}

/* Bound Range Exception Test
 * 
 * Asserts that the Bound Range exception is called appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
inline int Bound_Range_test(){
	TEST_HEADER;

	asm volatile("int $5");

	return PASS;

}

/* Invalid Opcode Exception Test
 * 
 * Asserts that the Invalid Opcode exception is called appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
inline int Invalid_Opcode_test(){
	TEST_HEADER;

	asm volatile("int $6");

	return PASS;

}

/* Sys Call Test
 * 
 * Asserts that Sys Calls are called appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Load IDT, IDT definition, Exception Handling
 * Files: x86_desc.h/S, idt_handlers.c/h, idt_exceptions.c/h
 */
inline int sys_call_test(){
	TEST_HEADER;

	asm volatile("int $128");

	return PASS;

}

/* Keyboard Test
 * 
 * Asserts that Keyboard interrupts correctly occur and are handled
 * appropriately. Also checks if scancodes are interpretered correctly
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: PIC initialization, PIC handling, Keyboard initialization, Keyboard handling
 * Files: i8259.c/h, keyboard.c/h, asm_wrappers.h/S, idt_handlers.c/h
 */
int keyboard_test(){
	TEST_HEADER;

	kb_init();

	return PASS;
}

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}

/* RTC Test
 * 
 * Asserts that RTC interrupts correctly occur and are handled
 * appropriately
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: PIC Initialization, PIC handling, RTC initialization, RTC handling
 * Files: i8259.c/h, rtc.c/h, asm_wrappers.h/S, idt_handlers.c/h
 */
int rtc_test(){
	TEST_HEADER;

	rtc_init();

	return PASS;
}

/* RTC and Keyboard Test
 * 
 * Asserts that RTC and Keyboard interrupts correctly occur and are handled
 * appropriately. Also checks if scancodes are interpreted correctly
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: PIC Initialization, PIC handling, RTC initialization, RTC handling,
 * Keyboard initialization, Keyboard handling
 * Files: i8259.c/h, rtc.c/h, keyboard.c/h, asm_wrappers.h/S, idt_handlers.c/h
 */
int rtc_kb_test(){
	TEST_HEADER;
	
	kb_init();
	rtc_init();

	return PASS;
}

/* Paging Values Test
 * 
 * Asserts that values are contained within the kernel memory
 * paging structures
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Paging initialization, Paging functionality
 * Files: kernel.c, paging.c/h, paging_helper.S/h
 */
int paging_test()
{
	TEST_HEADER;

	/* kernel mem start */
    char* pointer_to_kernel = (char*)0x400000;
    char deref = *pointer_to_kernel;

	/* kernel mem end */
	pointer_to_kernel = (char*)0x7FFFFF;
    deref = *pointer_to_kernel;

	/* video mem start */
	char* pointer_to_video = (char*)0xB8000;
	deref = *pointer_to_video;

	/* video mem end */
	pointer_to_video = (char*)0xB8FFF;
	deref = *pointer_to_video;

	return PASS;
}

/* Kernel Upper Bounds Test
 * 
 * Asserts that no values of kernel memory are contained
 * above the upper bounds of the paging structure
 * paging structures
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: Paging initialization, Paging functionality
 * Files: kernel.c, paging.c/h, paging_helper.S/h
 */
int kernel_out_of_bound_top()
{
	TEST_HEADER;

    char* pointer_to_kernel = (char*)0x3FFFFF;
	printf("%s \n", *pointer_to_kernel);
    // char deref = *pointer_to_kernel;

	return FAIL;
}

/* Kernel Lower Bounds Test
 * 
 * Asserts that no values of kernel memory are contained
 * below the upper bounds of the paging structure
 * paging structures
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: Paging initialization, Paging functionality
 * Files: kernel.c, paging.c/h, paging_helper.S/h
 */
int kernel_out_of_bound_bottom()
{
	TEST_HEADER;

    char* pointer_to_kernel = (char*)0x800000;
	printf("%s \n", *pointer_to_kernel);
    // char deref = *pointer_to_kernel;

	return FAIL;
}

/* Video Upper Bounds Test
 * 
 * Asserts that no values of video memory are contained
 * above the upper bounds of the paging structure
 * paging structures
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: Paging initialization, Paging functionality
 * Files: kernel.c, paging.c/h, paging_helper.S/h
 */
int video_out_of_bound_top()
{
	TEST_HEADER;

    char* pointer_to_video = (char*)0xB7FFF;
	printf("%s \n", *pointer_to_video);
    // char deref = *pointer_to_video;

	return FAIL;
}

/* Video Lower Bounds Test
 * 
 * Asserts that no values of video memory are contained
 * below the upper bounds of the paging structure
 * paging structures
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: Paging initialization, Paging functionality
 * Files: kernel.c, paging.c/h, paging_helper.S/h
 */
int video_out_of_bound_bottom()
{
	TEST_HEADER;

    char* pointer_to_video = (char*)0xB9000;
	printf("%s \n", *pointer_to_video);
    // char deref = *pointer_to_video;

	return FAIL;
}


/* Checkpoint 2 tests */

/* Terminal Write (Exact) Test
 * 
 * Asserts that a buffer is correctly written to the screen when the buffer
 * size and nbytes are equal (should Pass)
 * Inputs: None
 * Outputs: PASS if nbytes equals bytes written, FAIL if not
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_write_exact(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int bytes;	//bytes written
	int wbytes = 10;	//nbytes
	
	unsigned char test_buffer[10] = {'a', 'b', 'c', '1', '2', '3', '!', '@', '#', '~'};
	terminal.kb_buf_chars = wbytes;
	bytes = terminal_write(0, test_buffer, wbytes);
	// printf("%d", bytes);
	if(bytes != wbytes){	//check if bytes written equals bytes requested to be written
		result = FAIL;
	}
	return result;
}

/* Terminal Write (Under) Test
 * 
 * Asserts that a buffer is correctly written to the screen when the buffer
 * size is less than nbytes (should Fail) (truncates output)
 * Inputs: None
 * Outputs: PASS if nbytes equals bytes written, FAIL if not
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_write_under(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int buf_size = 10;
	int bytes;	//bytes written
	int wbytes = 5;	//nbytes
	
	unsigned char test_buffer[10] = {'a', 'b', 'c', '1', '2', '3', '!', '@', '#', '~'};
	terminal.kb_buf_chars = buf_size;
	bytes = terminal_write(0, test_buffer, wbytes);
	// printf("%d", bytes);
	if(bytes == wbytes){	//check if bytes written equals bytes requested to be written
		result = FAIL;
	}
	return result;	
}

/* Terminal Write (Over) Test
 * 
 * Asserts that a buffer is correctly written to the screen when the buffer
 * size is greater than nbytes (should Fail) (truncates output)
 * Inputs: None
 * Outputs: PASS if nbytes equals bytes written, FAIL if not
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_write_over(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int buf_size = 5;
	int bytes;	//bytes written
	int wbytes = 10;	//nbytes
	
	unsigned char test_buffer[5] = {'a', 'b', 'c', '1', '2'};
	terminal.kb_buf_chars = buf_size;
	bytes = terminal_write(0, test_buffer, wbytes);
	// printf("%d", bytes);
	if(bytes != wbytes){	//check if bytes written equals bytes requested to be written
		result = FAIL;
	}
	return result;	
}

// int terminal_read_only(){
// 	terminal_clear();

// 	TEST_HEADER;
// 	int result = PASS;
// 	int bytes;
// 	int rbytes = 11;
	
// 	unsigned char read_buffer[10] = {'a', 'b', 'c', '1', '2', '3', '!', '@', '#', '~'};

// 	// printf("Read Buffer: %s", read_buffer);
// 	terminal.kb_buf_chars = rbytes;
// 	bytes = terminal_read(0, read_buffer, rbytes);
// 	// printf("Bytes Read: %d\n", bytes);
// 	// printf("Read Buffer: %s", read_buffer);

// 	if(bytes != rbytes){
// 		result = FAIL;
// 	}
// 	return result;
// }


/* Terminal Read and Write (Exact) Test
 * 
 * Asserts that keyboard inputs are correctly read and written to the screen 
 * when the buffer size is equal to nbytes
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_read_write_exact(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int bytes_r = 0;	//bytes read
	int bytes_w = 0;	//bytes written
	int rbytes = 128;	//read nbytes
	int wbytes = 128;	//write nbytes
	int buf_size = 128;	//buffer size
	
	unsigned char read_buffer[128];

	while(1){
		terminal_open(0);
		bytes_r = terminal_read(0, read_buffer, rbytes);
		// printf("READ BUFFER: %s\n", read_buffer);
		bytes_w = terminal_write(0, read_buffer, wbytes);
		terminal_close(0);

		// printf("Bytes Read: %d, Bytes Wrote: %d\n", bytes_r, bytes_w);
	}

	if(buf_size != bytes_r || buf_size != bytes_w){	//check if bytes read/written equals buffer size
		result = FAIL;
	}

	return result;
}

/* Terminal Read and Write (Under) Test
 * 
 * Asserts that keyboard inputs are correctly read and written to the screen 
 * when the buffer size is greater than nbytes (truncates output)
 * Inputs: None
 * Outputs: PASS if nbytes equals bytes read and written, FAIL if not
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_read_write_under(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int bytes_r = 0;	//bytes read
	int bytes_w = 0;	//bytes written
	int rbytes = 25;	//read nbytes
	int wbytes = 25;	//write nbytes
	int buf_size = 128;	//buffer size greater than nbytes request, so will truncate
	
	unsigned char read_buffer[128];

	while(1){
		terminal_open(0);
		bytes_r = terminal_read(0, read_buffer, rbytes);
		// printf("READ BUFFER: %s\n", read_buffer);
		bytes_w = terminal_write(0, read_buffer, wbytes);
		terminal_close(0);

		// printf("Bytes Read: %d, Bytes Wrote: %d\n", bytes_r, bytes_w);
	}

	if(buf_size != bytes_r || buf_size != bytes_w){	//check if bytes read/written equals buffer size
		result = FAIL;
	}

	return result;
}

/* Terminal Read and Write (Over) Test
 * 
 * Asserts that keyboard inputs are correctly read and written to the screen 
 * when the buffer size is less than nbytes (will look the same as Exact)
 * Inputs: None
 * Outputs: PASS if nbytes equals bytes read and written, FAIL if not
 * Side Effects: None
 * Coverage: Terminal read/write, Keyboard handler
 * Files: terminal.c/h, keyboard.c/h, lib.c/h
 */
int terminal_read_write_over(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int bytes_r = 0;	//bytes read
	int bytes_w = 0;	//bytes written
	int rbytes = 150;	//read nbytes
	int wbytes = 150;	//write nbytes
	int buf_size = 128;	//buffer size less than nbytes request, so will truncate
	
	unsigned char read_buffer[128];

	while(1){
		terminal_open(0);
		bytes_r = terminal_read(0, read_buffer, rbytes);
		// printf("READ BUFFER: %s\n", read_buffer);
		bytes_w = terminal_write(0, read_buffer, wbytes);
		terminal_close(0);

		// printf("Bytes Read: %d, Bytes Wrote: %d\n", bytes_r, bytes_w);
	}

	if(buf_size != bytes_r || buf_size != bytes_w){	//check if bytes read/written equals buffer size
		result = FAIL;
	}

	return result;
}

/* RTC Frequency Test
 * 
 * Multi-stage test that asserts that valid RTC interrupt frequencies can occur
 * and invalid frequencies are handled appropriately
 * Stage 1: Tests 23Hz (should default to 2Hz from rtc_open and print)
 * Stage 2: Tests 4Hz
 * Stage 3: Tests 5Hz (should Fail and be skipped)
 * Stage 4: Tests 256Hz
 * Stage 5: Tests 1024Hz
 * Stage 6: Tests 2048Hz (should Fail and be skipped)
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: RTC open/read/write/close
 * Files: rtc.c/h
 */
int rtc_freq_test(){
	terminal_clear();

	TEST_HEADER;
	int result = PASS;
	int test_buf[1];

	rtc_open(0);
	
	// rtc_read(0, test_buf, 4);
	// rtc_write(0, test_buf, 4);

	int i;

	printf("Testing 23Hz (should default to 2Hz)\n");	//testing 23Hz, should default to 2Hz from rtc_open
	test_buf[0] = 23;
	for(i = 0; i < 50; i++){
		rtc_read(0, test_buf, 4);
		rtc_write(0, test_buf, 4);
		putc('0');
	}
	terminal_clear();

	printf("Testing 4Hz\n");	//testing 4Hz
	test_buf[0] = 4;
	for(i = 0; i < 30; i++){
		rtc_read(0, test_buf, 4);
		int temp = rtc_write(0, test_buf, 4);
		if(temp == 0){
			putc('1');
		}
		else{
			printf("Fail\n");
			break;
		}
	}
	terminal_clear();

	printf("Testing 5Hz (should Fail)\n");		//testing 5Hz, should skip
	test_buf[0] = 5;
	for(i = 0; i < 30; i++){
		rtc_read(0, test_buf, 4);
		int temp = rtc_write(0, test_buf, 4);
		if(temp == 0){
			putc('2');
		}
		else{
			printf("Fail\n");
			break;
		}
	}
	terminal_clear();

	printf("Testing 256Hz\n");	//testing 256Hz
	test_buf[0] = 256;
	for(i = 0; i < 1000; i++){
		rtc_read(0, test_buf, 4);
		int temp = rtc_write(0, test_buf, 4);
		if(temp == 0){
			putc('3');
		}
		else{
			printf("Fail\n");
			break;
		}
	}
	terminal_clear();

	printf("Testing 1024Hz\n");	//testing 1024Hz
	test_buf[0] = 1024;
	for(i = 0; i < 5000; i++){
		rtc_read(0, test_buf, 4);
		int temp = rtc_write(0, test_buf, 4);
		if(temp == 0){
			putc('4');
		}
		else{
			printf("Fail\n");
			break;
		}
	}
	terminal_clear();

	printf("Testing 2048Hz (should Fail)\n");	//testing 2048Hz, should skip
	test_buf[0] = 2048;
	for(i = 0; i < 5000; i++){
		rtc_read(0, test_buf, 4);
		int temp = rtc_write(0, test_buf, 4);
		if(temp == 0){
			putc('5');
		}
		else{
			printf("Fail\n");
			break;
		}
	}	

	rtc_close(0);

	return result;
}

/* read_data_test
 * 
 * Opens chosen file, and prints the contents of data 
 * 
 * Inputs: None
 * Outputs: 0 for success or -1 for failure 
 * Side Effects: None
 */
int read_data_test(){

	TEST_HEADER;
	
	const uint8_t* name; //The file name
	inode_t* inodeBlockPointer;	//Pointer that pointing to a specific inode 
	int fd = 0;

	int chooseFile = FRAMEZERO; //CHOOSE WHICH FILE I WANT TO TRY
	switch (chooseFile){
	case CAT:
		name = (uint8_t*)"cat";
		break;
	
	case COUNTER:
		name = (uint8_t*)"counter";
		break;
	
	case FISH:
		name = (uint8_t*)"fish";
		break;
	
	case FRAMEZERO:
		name = (uint8_t*)"frame0.txt";
		break;
	
	case FRAMEONE:
		name = (uint8_t*)"frame1.txt";
		break;
	
	case GREP:
		name = (uint8_t*)"grep";
		break;
	
	case HELLO:
		name = (uint8_t*)"hello";
		break;
	
	case LS:
		name = (uint8_t*)"ls";
		break;
	
	case PINGPONG:
		name = (uint8_t*)"pingpong";
		break;
	
	case SHELL:
		name = (uint8_t*)"shell";
		break;
	
	case SIGTEST:
		name = (uint8_t*)"sigtest";
		break;
	
	case SYSERR:
		name = (uint8_t*)"syserr";
		break;
	
	case TESTPRINT:
		name = (uint8_t*)"testprint";
		break;
	
	case VERYLONG:
		name = (uint8_t*)"verylargetextwithverylongname.tx";
		break;
	
	default:
		name = (uint8_t*)"oogabooga.txt";
		break;
	}

	int byte;
	uint8_t buf[BUF_SIZE];
	
	int value = file_open(name);

	if(value != 0){
		printf("File Does Not Exist");
		return -1;
	}

	inodeBlockPointer = (inode_t*)(fileSystemStartAddress + (1 + dentry_global.inodeNum) * MAX_BYTES_IN_BLOCK); //Getting us to point to the correct inode 
	int lengthOfFile = inodeBlockPointer -> length; 
	int bytesRead = file_read(fd, buf, lengthOfFile); // performing the read

	if(bytesRead == 0 || bytesRead == -1){ // we were not able to read this file
		printf("Did not read");
		return -1;
	}

	//Printing out each byte in the file
	for(byte = 0; byte < lengthOfFile; byte++){
		if(buf[byte] != '\0'){
      		putc(buf[byte]);
		}
	}

	value = file_close(fd); // closing file
	return PASS;
}

/* directory_test
 * 
 * Opens the directory and prints out information: file_name, file_type, file_size
 * 
 * Inputs: None
 * Outputs: PASS for success
 * Side Effects: None
 */
int directory_test()
{
	TEST_HEADER;

	/* setting up variables */
	int32_t i, j;
	int32_t nbytes = 0;
	int32_t fd = 0;
	uint8_t buf[32];
	const uint8_t* filename;
	dentry_t* specificDentry;
	inode_t* inodeBlockPointer;

	directory_open(filename); // opening indicated directory

	for (i = 0; i < blockZero->numOfDirectoryEntries; i++)
	{
		specificDentry = (dentry_t*)&(blockZero->dirEntries[i]);
		inodeBlockPointer = (inode_t*)(fileSystemStartAddress + ((1+specificDentry->inodeNum)*MAX_BYTES_IN_BLOCK)); //Getting pointer for correct inode block
		directory_read(fd, buf, nbytes);
		printf("file_name: ");

		// printing buffer which holds the file name
		for (j = 0; j < 32; j++) {
			if (buf[j] != NULL)
      			putc(buf[j]);
			else
				printf(" ");
    	}
	
		//Printing out the other information
		printf(", file_type: %d", specificDentry->fileType);
		printf(", file_size: %d", inodeBlockPointer->length);
		printf("\n");
	}

	directory_write(fd, buf, nbytes); // read-only system, so should return here

	directory_close(fd); // closing directory 

	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test())
	// launch your tests here
	// TEST_OUTPUT("Divide by Zero Test", divide_zero_test());
	// TEST_OUTPUT("Null Pointer Dereference Test", deref_test());
	// TEST_OUTPUT("NMI Exception Test", NMI_test());
	// TEST_OUTPUT("Overflow Exception Test", Overflow_test());
	// TEST_OUTPUT("Bound Range Exception Test", Bound_Range_test());
	// TEST_OUTPUT("Invalid Opcode Exception Test", Invalid_Opcode_test());
	// TEST_OUTPUT("Sys Call Test", sys_call_test());
	// TEST_OUTPUT("Keyboard Test", keyboard_test());	
	// TEST_OUTPUT("RTC Test", rtc_test());	
	// TEST_OUTPUT("RTC Test and Keyboard Test", rtc_kb_test());
	// TEST_OUTPUT("Paging Test", paging_test());
	// TEST_OUTPUT("Kernel Upper Bounds Test", kernel_out_of_bound_top());
	// TEST_OUTPUT("Kernel Lower Bounds Test", kernel_out_of_bound_bottom());
	// TEST_OUTPUT("Video Upper Bounds Test", video_out_of_bound_top());
	// TEST_OUTPUT("Video Lower Bounds Test", video_out_of_bound_bottom());

	/* Checkpoint 2 */
	// TEST_OUTPUT("Terminal Write Test (Exact)", terminal_write_exact());
	// TEST_OUTPUT("Terminal Write Test (Under)", terminal_write_under());
	// TEST_OUTPUT("Terminal Write Test (Over)", terminal_write_over());
	// TEST_OUTPUT("Terminal Read/Write Test (Exact)", terminal_read_write_exact());
	// TEST_OUTPUT("Terminal Read/Write Test (Under)", terminal_read_write_under());
	// TEST_OUTPUT("Terminal Read/Write Test (Over)", terminal_read_write_over());
	// TEST_OUTPUT("RTC Frequency Test", rtc_freq_test());
	// TEST_OUTPUT("Directory Test", directory_test());	
	// TEST_OUTPUT("Read Data Test", read_data_test());	
}
