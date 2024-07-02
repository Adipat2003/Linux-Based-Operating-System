 #include "sys_call.h"
 
int isShell[6]; //Used as a checker if what we are running is a shell or not
int PIDArray[6]; //Used as a checker to know which PID we should be using
uint32_t currentPID = 0; //Global variable for the currentPID (Used as a counter)


/* Stdin function pointer table */
static fops_t stdin_tmp = {
    (ReadFuncs) terminal_read,
    (WriteFuncs) bad_write,
    (OpenFuncs) bad_open,
    (CloseFuncs) bad_close
};

/* Stdout function pointer table */
static fops_t stdout_tmp = {
    (ReadFuncs) bad_read,
    (WriteFuncs) terminal_write,
    (OpenFuncs) bad_open,
    (CloseFuncs) bad_close
};

/* rtc function pointer table */
static fops_t rtc_tmp = {
    (ReadFuncs) rtc_read,
    (WriteFuncs) rtc_write,
    (OpenFuncs) rtc_open,
    (CloseFuncs) rtc_close
};

/* directory function pointer table */
static fops_t directory_tmp = {
    (ReadFuncs) directory_read,
    (WriteFuncs) directory_write,
    (OpenFuncs) directory_open,
    (CloseFuncs) directory_close
};

/* directory function pointer table */
static fops_t file_tmp = {
    (ReadFuncs) file_read,
    (WriteFuncs) file_write,
    (OpenFuncs) file_open,
    (CloseFuncs) file_close
};

/* bad -1 return function pointer table */
static fops_t bad_tmp = {
    (ReadFuncs) bad_read,
    (WriteFuncs) bad_write,
    (OpenFuncs) bad_open,
    (CloseFuncs) bad_close
};


/* Halt
 * 
 * Stops currently running process to go back to the parent program
 * 
 * Inputs: status -- How the program was terminated
 *          0 - Successful
 *          1- Failure
 *          69 - Exception
 * Outputs: Goes back to the shell user space
 * Side Effects: None
 */
int32_t halt(uint8_t status){
    int i;
    int fd;
    //while(curr_ter != active_terminal){}
    cli();
    
    if(terminals[active_terminal].pids[0] == -1){
        return 0;
    }
    uint32_t new_status = (uint32_t)status; //cast status from 8 bits to 32
    if(exception_flag == 1){    //check if exception has been raised
        new_status = 256;   //set status to 256 if exception raised
        exception_flag = 0; //set exception flag low
    }
    
    //Restore parent data
    PCB_t* curr_pcb = get_PCB(currentPID);

    //We dont want to halt the current process so we just have to setup the IRET for shell again
    if (currentPID == 0 || currentPID == 1 || currentPID == 2){
        uint32_t eipArg = curr_pcb->current.temp_eip;
        uint32_t espArg = curr_pcb->current.temp_esp;
        setup_IRET(eipArg, espArg);
    }

    //Making the index for the PID we are using back to being free to use
    PIDArray[currentPID] = 0;
    isShell[currentPID] = 0;
    terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1] = -1;
    terminals[curr_ter].pid_idx--;

    //Getting the PCB Address of the parent program
    PCB_t* parent = get_PCB(curr_pcb->parent_id);
    
    //Setting the currentPID back to the parents
    currentPID = curr_pcb->parent_id;

    //restore Parent Paging
    new_program(currentPID);
    flush_tlb();

    //close fds for that specific pcb (Skipping the stdin and stdout)
    for (fd = 2; fd < 8; fd++){
        close(fd);
    }

    //Setting up the jump to execute paramters 
    tss.ss0 = parent->current.tss_ss0;
    tss.esp0 = (eightMB - ((parent->process_id) * eightKB)) - sizeof(int32_t);
    
    //Getting back the correct ESP and EBP for going back to the parent
    uint32_t parentEBPArg = curr_pcb->current.saved_ebp;
    uint32_t parentESPArg = curr_pcb->current.saved_esp;
    parent->current.exec_ebp = parentEBPArg;
    parent->current.exec_esp = parentESPArg;

    sti();

    jump_to_execute(new_status, parentESPArg, parentEBPArg);
    
    return 0;
}

/* Execute
 * 
 * Executes a typed input following the steps:
 * 1. Parsing Args
 * 2. Check for executable
 * 3. Set up paging
 * 4. Load file into memory
 * 5. Create PCB
 * 6. Prepare for Context Switch
 * 7. Push IRET context to kernel stack 
 * Inputs: command, the input we want to be running, either garbage or executable
 * Outputs: Running the user program if it is an executable file
 * Side Effects: None
 */
int32_t execute(const uint8_t* command){
    cli();
    //Declare variables
    int i;
    int value;
    uint8_t userBuf[4]; //Holds 4 bytes after reading file
    uint8_t exeName[32]; //File names can be up to 32 characters 
    uint8_t argName[32];
    int commandEntry = 0;
    int argEntry = 0;
    int exeEntry = 0;
    int temp = currentPID;
    
    if(terminals[active_terminal].pid_idx >= 4){
        return -1;
    }

    if(PIDs_left() <= (!status2 + !status3)){
        return -1;
    }

    //Making the arrays just have initially all null terminating
    for(i = 0; i < 32; i++){
        exeName[i] = '\0';
        argName[i] = '\0';
    }

    //Find until we have any leading spaces
    while(command[commandEntry] == SPACE){
        commandEntry++;
    }
    
    if(command[0] == '\n' || command[0] == NULL){
        return 0;
    }

    //I will need to go through our command argument, keep filling in exeName
    //With all thats in command array until I see 0x20(space) which means I have the current exeName
    while(command[commandEntry] != SPACE && command[commandEntry] != '\n' && command[commandEntry] != '\0'){  //IS IT 0x20 OR ' '
        if(exeEntry > 31){
            break;
        }
        exeName[exeEntry] = command[commandEntry];  
        commandEntry++;
        exeEntry++;
    }

    //Find until we have any leading spaces
    while(command[commandEntry] == SPACE){
        commandEntry++;
    }

    //Filling in the argument array until we reached the end of our command input
    while(command[commandEntry] != '\n' && command[commandEntry] != '\0' && command[commandEntry] != SPACE){
        if(argEntry > 31){
            break;
        }
        argName[argEntry] = command[commandEntry];
        argEntry++;
        commandEntry++;
    }
    
    //Fill in the dentry global variable with the corresponding executible name
    value = read_dentry_by_name(exeName, &dentry_global);
    if(value == -1){
        return -1;
    }
    
    //I want to get the first 4 bytes of the file and put it into my userBuf
    value = read_data(dentry_global.inodeNum, 0, userBuf, 4);
    if(value == -1){
        return -1;
    }

    //Check if the first bytes that are in the userBuf correspond to an elf file
    if(userBuf[0] != PERIOD || userBuf[1] != E || userBuf[2] != L || userBuf[3] != F){
        return -1;
    }
    int flag = 1;
    //Go through the PID array to find the avaliable 
    for(i = 0; i < 6; i++){
        //Check if the index for the PID array is free(0), if so then we can use that index as ourcurrent PID
        if(PIDArray[i] == 0){
            currentPID = i;
            PIDArray[i] = 1; //Saying that that specific entry is busy
            flag = 0;
            break;
        }
    }

    if(flag == 1){
        return -1;
    }
    
    //Making sure the current PID is not equal to the old PID
    if (currentPID != 0){
        if (currentPID == temp)
            return -1;
    }

    //Physical Memory starts at: 8 MB + (PID# * 4MB)
    //uint32_t physStartMemory = 0x800000 + (currentPID * 0x400000);
    new_program(currentPID); // NEED TO CHECK, COULD BE WRONG
    flush_tlb();
    
    //Step 4: Load file into memory
    //I need to fill in the contents of the file I am reading 
    //Starting at memory address 0x08048000
    inodePointer = fileSystemStartAddress + BYTES_IN_BLOCK; //Make the inode point to the block thats after the boot block
    inode_t* inodeBlockPointer = (inode_t*)(inodePointer + ((dentry_global.inodeNum) * BYTES_IN_BLOCK)); //This gets us to point to correct inode block
    uint32_t lengthOfFile = inodeBlockPointer->length; //How many bytes are in file
    value = read_data(dentry_global.inodeNum, 0, (uint8_t*)(VIRTUAL_START_ADDRESS), lengthOfFile);
    if(value == -1){
        return -1;
    }

    //Need to use currentPID to index into isShell 
    if(exeName[0] == 's' && exeName[1] == 'h' && exeName[2] == 'e' && exeName[3] == 'l' && exeName[4] == 'l'){
        isShell[currentPID] = 1;
    }

    //Step 5: Creating the PCB
    //Getting the address of the PCB that we are currently working with depending on the PID
    PCB_t* PCBAddress = get_PCB(currentPID);
    PCBAddress->process_id = currentPID;

    terminals[active_terminal].pids[terminals[active_terminal].pid_idx] = currentPID;

    //Assigning the parents for the corresponding current PID
    if(terminals[active_terminal].pid_idx == 0){
        //If it a base shell, make sure you the parent of the shell is the same one
        PCBAddress->parent_id = terminals[active_terminal].pids[0]; //Should either be 0, 1 , 2 depending on which terminal array we work with
    }
    else{
        PCBAddress->parent_id = terminals[active_terminal].pids[terminals[active_terminal].pid_idx -1];
    }

    terminals[active_terminal].pid_idx++;
     
    //Filling in the contents of the file descriptor contents
    for(i = 2; i < 8; i++){
        PCBAddress->file_descriptors[i].fileTablePointer = &bad_tmp;
        PCBAddress->file_descriptors[i].inode = 0;
        PCBAddress->file_descriptors[i].filePos = 0;
        PCBAddress->file_descriptors[i].flags = 0;
    }

    //Fill in the correct PCBArray's process descriptor table
    //stdin and stdout -- may need to assign other fops to specific functions for stdin and stdout
    PCBAddress->file_descriptors[0].fileTablePointer = &stdin_tmp;
    PCBAddress->file_descriptors[0].inode = 0;
    PCBAddress->file_descriptors[0].filePos = 0;
    PCBAddress->file_descriptors[0].flags = 1;

    PCBAddress->file_descriptors[1].fileTablePointer = &stdout_tmp;
    PCBAddress->file_descriptors[1].inode = 0;
    PCBAddress->file_descriptors[1].filePos = 0;
    PCBAddress->file_descriptors[1].flags = 1;

    //Putting the argName array into the PCB struct we are working with 
    memcpy((uint8_t*)PCBAddress->argument, (uint8_t*)argName, MAX_FILENAME_LENGTH);

    //Step 6: Context Switch
    //IRET needs 5 elements on the stack:
    //User DS, ESP, EFLAGS, CS, EIP
    //First lets find EIP:
    //I want to be looking at bytes 24-27 which store the address within our user program where to start executing
    value = read_data(dentry_global.inodeNum, 24, userBuf, sizeof(int32_t)); //IS THIS 24 or 23
    if(value == -1){
        return -1;
    }

    //store whats inside of userBuf into a eip
    uint32_t eipArg = *((unsigned int *)userBuf);
    
    //Next, lets find the ESP Arg 
    uint32_t espArg = MB128 + MB4 - sizeof(int32_t);
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (eightMB - ((currentPID) * eightKB)) - sizeof(int32_t);

    PCBAddress->current.tss_esp0 = tss.esp0;
    PCBAddress->current.tss_ss0 = tss.ss0;

    PCBAddress->current.temp_esp = espArg;
    PCBAddress->current.temp_eip = eipArg;

    //getting the correct esp and ebp to be used to halt back
    unsigned int ebp, esp;
    asm("\t movl %%esp, %0" : "=r"(esp));
    asm("\t movl %%ebp, %0" : "=r"(ebp));
    PCBAddress->current.saved_esp = esp;
    PCBAddress->current.saved_ebp = ebp;

    PCBAddress->current.exec_esp = esp;
    PCBAddress->current.exec_ebp = ebp;

    // PCBAddress->current.saved_ebp = PCBAddress->current.exec_ebp;
    // PCBAddress->current.saved_esp = PCBAddress->current.exec_esp;

    sti();
    while((curr_ter != active_terminal) && flag3 == 0){}
    setup_IRET(eipArg, espArg);
    return 0;
}

/* Read
 * 
 * The system call read function that calls other read files such as file/directory/rtc/keyboard read
 * Inputs: fd -- file descriptor index for the file descriptor array
 *         buf -- Used to contain all the bytes that are read or file name contents
 *         nbytes -- Used to know how many bytes need to be read for the corresponding function
 * Outputs: returns the bytes that were read from the read function
 * Side Effects: None
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    sti();
    int bytes_read = 0;
    PCB_t* curr_pcb = get_PCB(currentPID);

    /* Ensuring we cannot read from an invalid fd or a closed file */
    if(fd < 0 || fd >= 8 || curr_pcb->file_descriptors[fd].flags == 0){
        return -1;
    }

    bytes_read = curr_pcb->file_descriptors[fd].fileTablePointer->fread(fd, buf, nbytes);

    //Need to check if we are at a directory file
    if((curr_pcb->file_descriptors[fd].flags >> 8) == 1){ //00 00 01 00 -> 00 00 00 01
        curr_pcb->file_descriptors[fd].filePos = 0;
        return bytes_read;
    }

    curr_pcb->file_descriptors[fd].filePos += bytes_read; //Increment file position to however many bytes we read
    return bytes_read;
}

/* Write
 *
 * Writing to the either the RTC/Directory/File/Terminal
 * Specifically forcused on terminal to write stuff to screen
 * 
 * Inputs: int32_t fd -- fd of opened file we wish to write from
 *         const void* buf -- buffer we wish to write to screen
 *         int32_t nbytes -- number of bytes we wish to write
 * Outputs: 0 for successful write, -1 for failed write
 * Side Effects: Calls close function associated with file
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t* curr_pcb = get_PCB(currentPID);

    /* ensuring we are not writing to an invalid fd or to a file that is currently closed */
    if(fd < 0 || fd >= 8 || curr_pcb->file_descriptors[fd].flags == 0){
        return -1;
    }
   
    return curr_pcb->file_descriptors[fd].fileTablePointer->fwrite(fd, buf, nbytes);
}

/* Open
 * 
 * System Call function that opens the desired file name
 * Inputs: const uint8_t* filename -- name of the file we wish to open
 * Outputs: fd_idx -- available index into the current PCB's filedescriptor array to fill file info with, if any are open
 * Side Effects: Fills the current PCB's fd array with the file we are trying to open, if able to
 */
int32_t open(const uint8_t* filename){
    int fd_idx = 0;
    if(read_dentry_by_name(filename, &dentry_global) == -1){    //ensuring that file name is valid
        return -1;
    }
    fileType = dentry_global.fileType;       

    if(fileType < 0 || fileType > 2){ //Invalid type of file so return failure
        return -1;
    }

    PCB_t* curr_pcb = get_PCB(currentPID);

    //go through each file_descriptor and check if flag is active, find first one that is not and make that the fd we will fill
    for(fd_idx = 2; fd_idx < 8; fd_idx++){
        if(curr_pcb->file_descriptors[fd_idx].flags >= 1){
            continue;
        }
        else{
            break;
        }
    }

    if(fd_idx >= 8){ //This means we checked every fd for this PCB and all were filled already so return fail
        return -1;
    }
    
    curr_pcb->file_descriptors[fd_idx].inode = dentry_global.inodeNum;
    curr_pcb->file_descriptors[fd_idx].filePos = 0;
    
    //RTC = 0
    //Directory = 1
    //Regular File = 2
    //Assigning file with its associated set of fops
    //Flags are 4 Byte: unused, unused, file type, active
    if(fileType == 0){
       curr_pcb->file_descriptors[fd_idx].flags = RTC_FLAGS; 
       curr_pcb->file_descriptors[fd_idx].fileTablePointer = &rtc_tmp;
    }
    else if(fileType == 1){
        curr_pcb->file_descriptors[fd_idx].flags = DIRECTORY_FLAGS;
        curr_pcb->file_descriptors[fd_idx].fileTablePointer = &directory_tmp;
    }
    else{
        curr_pcb->file_descriptors[fd_idx].flags = FILE_FLAGS;
        curr_pcb->file_descriptors[fd_idx].fileTablePointer = &file_tmp;
    }

    //call actual open function for file now that it is assigned
    curr_pcb->file_descriptors[fd_idx].fileTablePointer->fopen(filename);

    return fd_idx;
}

/* Close
 * 
 * Close any file for the corresponding file descriptor index
 * Inputs: int32_t fd -- filedescriptor for opened file we wish to close
 * Outputs: 0 for successful close, -1 for failed close
 * Side Effects: Calls associated close, then sets the fd of current PCB to be marked inactive and assigns fd info with zeros  
 */
int32_t close(int32_t fd){
    PCB_t* curr_pcb = get_PCB(currentPID); //acquire correct PCB block 

    //Ensuring we cannot close stdin/stdout, or close an invalid or already closed fd
    if(fd < 2 || fd >= 8 || curr_pcb->file_descriptors[fd].flags == 0){
        return -1;
    }

    //call appropriate close function and make fd it was using available for later use
    curr_pcb->file_descriptors[fd].fileTablePointer->fclose(fd);
    curr_pcb->file_descriptors[fd].fileTablePointer = &bad_tmp;
    curr_pcb->file_descriptors[fd].filePos = 0;
    curr_pcb->file_descriptors[fd].flags = 0;
    curr_pcb->file_descriptors[fd].inode = 0;
    
    return 0;
}


/* Getargs
 * 
 * Reads the program's command line argument into a user-level buffer where the we get the argument from execution
 * Inputs:  buf -- store the command line argument into the program
 *          nbytes -- the amount of the bytes that need to be in the buffer 
 * Outputs: 0 for success, -1 for failure
 * Side Effects: Executable can now take arguments like cat frame0.txt
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
    PCB_t* curr_pcb = get_PCB(currentPID);
    uint8_t* executableArgument = curr_pcb->argument;
    
    //Check if our argument is just NULL
    if(executableArgument[0] == NULL || nbytes < 0 || buf == NULL){
        return FAIL;
    }

    //Putting our argument into the user-level buffer
    memcpy((uint8_t*)buf, (uint8_t*)executableArgument, nbytes);

    return 0;
}


/* Vidmap
 * 
 * The vidmap call maps the text-mode video memory into user space at a pre-set virtual address. If the location is invalid, the 
 * call should return -1. Setting video memory accurately mapping to the page directory and table, and drawing onto the page. Flush
 * the TLB after to make sure that the page is reset such that switching between kernel and user is okay.
 * Inputs: screen_start -- stores the virtual address pointing to the page directory
 * Outputs: 0 on success and -1 on failure
 * Side Effects: Draws to the screen 
 */
int32_t vidmap(uint8_t** screen_start){

    // screen start needs to be accurate
    if(screen_start == NULL){
        return FAIL;
    }

    // screen start should be within the bounds
    if ((int32_t) screen_start < vidmap_start || (int32_t) screen_start > vidmap_end)
        return FAIL;

    
    vidmap_table[0].present = 1;
    vidmap_table[0].user_supervisor = 1;
    vidmap_table[0].addr_31_12 = VIDEO >> shift_12;
   
    
    page_directory[33].KB_four.present = 1;                                                  // valid entry
    page_directory[33].KB_four.read_write = 1;                                               // writable memory 
    page_directory[33].KB_four.user_supervisor = 1;                                          // user has access
    page_directory[33].KB_four.write_through = 0;
    page_directory[33].KB_four.cache_disable = 0;
    page_directory[33].KB_four.accessed = 0;
    page_directory[33].KB_four.available_2 = 0;
    page_directory[33].KB_four.page_size = 0;
    page_directory[33].KB_four.available = 0;
    page_directory[33].KB_four.addr_31_12 = (((unsigned int)(vidmap_table)) >> shift_12);    // mapping the page table to the page directory

    *screen_start = (uint8_t*)SCREEN_MEM;                                                           // (69 * 2^22 ) bytes to hex

    flush_tlb();                                                                                    // making sure nothing messes up when switching between user and kernel

    return 0;                                                                                       // success
}

/* get_PCB
 * 
 * Gets the address of the PCB for corresponding process
 * Inputs: pid - the process id for the process that is running
 * Outputs: Returns the PCB address 
 * Side Effects: None
 */
PCB_t* get_PCB(uint32_t pid){
    //From bootup do this
    if(pid == -1){
        if(curr_ter == 0){
            pid = 0;
        }
        else if(curr_ter == 1){
            pid = 1;
        }
        else if(curr_ter == 2){
            pid = 2;
        }
    }
    return (PCB_t *)(eightMB - ((pid+1) * eightKB));
}

int32_t PIDs_left(){
    int i;
    int pids_left = 0;
    for(i = 0; i < 6; i++){
        //Check if the index for the PID array is free(0), if so then we can use that index as ourcurrent PID
        if(PIDArray[i] == 0){
            pids_left++;
        }
    }

    return pids_left;
}
