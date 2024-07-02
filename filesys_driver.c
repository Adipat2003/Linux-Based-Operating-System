#include "filesys_driver.h"

#define MAX_FILE_LENGTH     32
#define MAX_FILE_AMOUNT     64
#define MAX_BYTES_IN_BLOCK  4096

int32_t offsetStorage[MAX_FILE_AMOUNT];
unsigned int fileSystemStartAddress;
dentry_t dentry_global;

/* filesys_init
 * 
 * Gets where the file system is starting at and intiailize our offsetStorages to -1 initially
 * 
 * Inputs: None
 * Outputs: None
 * Side Effects: Sets up blockZero for usage 
 */
void filesys_init(){
    //Get where the file system is starting which is block 0, each block is 4096 bytes away from eachother
    blockZero = (bootBlockT*)(fileSystemStartAddress);
    memset(offsetStorage, -1, MAX_FILE_AMOUNT);
}

/* read_dentry_by_name
 * 
 * Sets up the global dentry so we can use that to access a specific file's information through its file name
 * Inputs:
 *      fname = the name of the file we are looking at
 *      dentry = dentry_global which is going to be filled to be used for later 
 * Outputs: 0 for success, -1 for failure
 * Side Effects: Sets up dentry_global variable
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    
    int i;

    if (fname == NULL)
        return -1;

    //GOing through all possible files in our operating system
    for (i = 0; i < MAX_FILE_AMOUNT; i++)
    {
        //Checking if the fileName from our directory entry matches what is passed in, and bound checking, if good, call read_dentry_by_index for specific directory entry to fill in dentry_global
        if (!strncmp((int8_t*)blockZero->dirEntries[i].fileName, (int8_t*)fname, MAX_FILE_LENGTH) && strlen((int8_t*)fname) <= MAX_FILE_LENGTH) 
        {
            if(read_dentry_by_index(i, dentry)){
                return -1;
            }
            
            return 0;
        }
    }

    return -1;
}

/* read_dentry_by_index
 * 
 * Sets up the global dentry so we can use that to access a specific file's information through specific index in the boot blocks directory
 * Inputs: 
 *      index = specific directory entry we are looking at
 *      dentry = dentry_global which is going to filled with a file's information
 * Outputs: 0 for success, -1 for failure
 * Side Effects: 
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

    //Checking if index passed is correct
    if (index >= blockZero->numOfDirectoryEntries || index < 0){
        return -1;
    }
    
    //Filling the dentry_global with the file we are looking fileName, fileType, and inodeNum
    strncpy((int8_t*)dentry->fileName, (int8_t*)blockZero->dirEntries[index].fileName, MAX_FILE_LENGTH);
    dentry->fileType = blockZero->dirEntries[index].fileType;
    dentry->inodeNum = blockZero->dirEntries[index].inodeNum;

    return 0;
}

/* read_data
 * 
 * Reading a file's/inode's byte data stored inside each of its data blocks
 * Inputs: 
 *      inode = the specific inode # we are looking at corresponding to one specific file
 *      offset = where we are starting off our reading data process
 *      buf = buffer used to put the read bytes into
 *      length = how many bytes of information that we want to be reading in specific call
 * Outputs: -1 for failure, or the number of bytes that were read through the call
 * Side Effects: None
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    //Check if our inode value is out of bounds
    if(inode > blockZero->numOfInodes || inode < 0){
        // printf("This has reached\n");
        return -1;
    }

    //declare variables
    int byte;
    totalInodes = blockZero->numOfInodes;
    totalDataBlocks = blockZero->numOfDataBlocks;
    inodePointer = fileSystemStartAddress + MAX_BYTES_IN_BLOCK; //Make the inode point to the block thats after the boot block
    dataBlockPointer = (fileSystemStartAddress + ((totalInodes+1)* MAX_BYTES_IN_BLOCK)); //Points to the data block information which is right after all of the inode blocks in memory(N+1)
    inode_t* inodeBlockPointer = (inode_t*)(inodePointer + (inode * MAX_BYTES_IN_BLOCK)); //This gets us to point to correct inode block
    uint32_t lengthOfFile = inodeBlockPointer->length; //How many bytes are in file
    uint32_t bytesRead = 0; //A counter for the amount of bytes you read which is what you are returning
    uint8_t* dataBlockNumPointer; //Each data block # corresponds to a pointer to some memory block, this pointer is going to be used to help point to that data block we are working with
    uint32_t blockDataNum; //Which block are we at working with
        
    uint32_t byteIndex = offset % MAX_BYTES_IN_BLOCK; //Since each block holds 4096 bytes, then modding it with 4096, will tell you how many bytes we have current read for that specific block
                                        //When our byte index is greater than 4096, then we have reset our byte index, meaning we are on the next block

    //For data block
    uint32_t blockIndex = offset / MAX_BYTES_IN_BLOCK; //Since each block is 4096 bytes, our offset will go until we have reached the amount of bytes all in the file. 
                                            //When we have a number in our offset like 5000, this helps us determine thats block 1

        

    //Check if our offset exceeds the length of file, that means nothing should really happen
    if(offset >= lengthOfFile){
        return 0;
    }

    blockDataNum = inodeBlockPointer->numOfDataBlocks[blockIndex];

    if (blockDataNum >= blockZero->numOfDataBlocks){
            return -1;
    }
            
    //lets start going through the data, until we reach the amount of bytes that we are supposed to read and copy the data block information into the buf, byte by byte 
    for(byte = 0; byte < length; byte++){
            
        //Need to make sure we don't exceed the amount of bytes read for given file
        if(byte + offset >= lengthOfFile){
            return bytesRead;
        }

        //When we on a new block, we have to make sure we clear our byteIndex indicated we are on a new block, resetting it back to 0 when necessary
        if (byte != 0){
            if((byte + offset) % MAX_BYTES_IN_BLOCK == 0){ //If result is 0, then that means new block, therefore lets reset our index
                byteIndex = 0; //The start of the next block 
                blockIndex++;   //Increasing counter of the block index when Im starting to look at new block
            }
        }

        blockDataNum = inodeBlockPointer->numOfDataBlocks[blockIndex]; //Now we know what data block we are looking at rn
        //Check if we have a naughty data block
        if(blockDataNum >= totalDataBlocks){
            return -1;
        }
            
        //Now we need to find where these data blocks are pointing to then use memcpy each individual byte for specific block we are at
        dataBlockNumPointer = (uint8_t*)(dataBlockPointer + (blockDataNum * MAX_BYTES_IN_BLOCK) + byteIndex); //Our block number * 4096 gets us to point to the first byte to the correspond block we are at, and the + index will get us the 
                                                                                //correct byte we need to work with. We need the + dataBlockPointer because that is pointing to the very data block after the inode information
                                                                                //in memory(N+1)
    
        memcpy(buf+bytesRead, dataBlockNumPointer, 1); //Puts the byte that we are going to read into the buf 
        bytesRead++;
        byteIndex++;
    }
        
    return bytesRead;
}


//File functions

/* file_read
 * 
 * Reads the specified amount of bytes from the passed value by calling read_data which fills in
 * those bytes read into the buffer
 * 
 * Inputs: 
 *      fd = file directory(Not used for CP2)
 *      buf = Buffer we put the bytes we read into
 *      nbytes = the number of bytes we want to read in the file
 * Outputs: Returns the number of bytes read from the file
 * Side Effects: None
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
    if(buf == NULL){
        return -1;
    }

    PCB_t* PCBIndex = get_PCB(currentPID);

    int numBytesRead = read_data(PCBIndex->file_descriptors[fd].inode, PCBIndex->file_descriptors[fd].filePos, buf, nbytes);

    if(numBytesRead == -1){
        return -1;
    }

    return numBytesRead;
}

/* file_write
 * 
 * Does nothing because read only system
 * 
 * Inputs: fd, buf, nbytes
 * Outputs: -1 for failure
 * Side Effects: None
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* file_open
 * 
 * Initialize the dentry_global to the corresponding file we are at through read_dentry_by_name
 * 
 * Inputs: Filename which is which file you want to open up 
 * Outputs: 0 for success or -1 for failure 
 * Side Effects: changes the dentry_global variables values so we can access files correctly depending on situation
 */
int32_t file_open(const uint8_t* filename){
    return 0;
}

/* file_close
 * 
 * Undos what is done in open function, but does nothing in reality
 * 
 * Inputs: fd
 * Outputs: 0 for success
 * Side Effects: None
 */
int32_t file_close(int32_t fd){
    return 0;
}


//Directory functions

/* directory_read
 * 
 * Reads a specified directory entry from the global dirIndex variable and puts inside buffer
 * 
 * Inputs: 
 *      fd = not used
 *      buf = Contains each of the file's filenames 
 *      nbytes = not used
 * Outputs: returns the length of the filename 
 * Side Effects: 
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes){

    dentry_t dentry_temp2; //Used to contain a files information using a specific Index
    // int PCBIndex = get_PCB();
    if(dirIndex >= blockZero->numOfDirectoryEntries){
        return 0;
    }

    if(read_dentry_by_index(dirIndex, &dentry_temp2)){  //Using the global variable that we keep increasing, it gets us to look at a new directory entry
        return -1;
    }
        
    memcpy((int8_t*)buf, dentry_temp2.fileName, MAX_FILE_LENGTH);   //Putting the fileName we are looking at into the buffer

    uint32_t lengthOfFileName = strlen(dentry_temp2.fileName);  //Getting the length of File and returning that
    dirIndex += 1;

    if(lengthOfFileName > MAX_FILE_LENGTH){
        return MAX_FILE_LENGTH;
    }

    else{
       return lengthOfFileName; 
    }
}

/* directory_write
 * 
 * Does nothing because read only system
 * 
 * Inputs: fd, buf, nbytes
 * Outputs: -1 for failure
 * Side Effects: None
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* directory_open
 * 
 * Sets our index counter to 0 which is to be used for the directory indexes to read them
 * 
 * Inputs: fd
 * Outputs: 0 for success
 * Side Effects: None
 */
int32_t directory_open(const uint8_t* filename){
    dirIndex = 0;
    return 0;
}

/* directory_close
 * 
 * Closes the directory, does nothing  
 * Inputs: fd
 * Outputs: 0 for success
 * Side Effects: None
 */
int32_t directory_close(int32_t fd){
    return 0;
}
