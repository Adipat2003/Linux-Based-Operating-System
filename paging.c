#include "paging.h"

/* Paging Initializer
 * 
 * Initializes the memeory in the intented manner i.e. 
 * sets up the first KB of memory and the 1 MB of Kernel memory
 * through the page directory union struct. Also sets up the video 
 * memory address as specified (0xB8000). Calls the assembly code to 
 * enable both 4KB and 4MB paging
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void page_init()
{
    int i;
    for (i = 0; i < num_entries; i++)
    {
        /* defining the first KB of memory to present */
        if (i == 0)
        {
            page_directory[i].KB_four.present = 1;                                                   // valid entry
            page_directory[i].KB_four.read_write = 1;                                                // writable memory 
            page_directory[i].KB_four.user_supervisor = 0;
            page_directory[i].KB_four.write_through = 0;
            page_directory[i].KB_four.cache_disable = 0;
            page_directory[i].KB_four.accessed = 0;
            page_directory[i].KB_four.available_2 = 0;
            page_directory[i].KB_four.page_size = 0;
            page_directory[i].KB_four.available = 0;
            page_directory[i].KB_four.addr_31_12 = (((unsigned int)(page_table)) >> shift_12);        // pointer to the memory 
        }

        /* one MB of kernel memory */
        else if (i == 1)
        {
            page_directory[i].MB_four.present = 1;                                                   // valid entry
            page_directory[i].MB_four.read_write = 1;                                                // writable memory
            page_directory[i].MB_four.user_supervisor = 0;
            page_directory[i].MB_four.write_through = 0;
            page_directory[i].MB_four.cache_disable = 0; 
            page_directory[i].MB_four.accessed = 0;
            page_directory[i].MB_four.dirty = 0;
            page_directory[i].MB_four.page_size = 1;                                                 // four MB config
            page_directory[i].MB_four.global = 1;                                                    // making it global
            page_directory[i].MB_four.available = 0;
            page_directory[i].MB_four.pat = 0;
            page_directory[i].MB_four.space = 0;
            page_directory[i].MB_four.addr_31_22 = 1;
        }

        /* general memory, technically don't need to initialize */
        else
        {
            page_directory[i].MB_four.present = 0;
            page_directory[i].MB_four.read_write = 1;
            page_directory[i].MB_four.user_supervisor = 0;
            page_directory[i].MB_four.write_through = 0;
            page_directory[i].MB_four.cache_disable = 0; 
            page_directory[i].MB_four.accessed = 0;
            page_directory[i].MB_four.dirty = 0;
            page_directory[i].MB_four.page_size = 1;                                                 // four MB config
            page_directory[i].MB_four.global = 0;                                                    // making it global
            page_directory[i].MB_four.available = 0;
            page_directory[i].MB_four.pat = 0;
            page_directory[i].MB_four.space = 0;
        }
    }

    page_directory[33].MB_four.present = 1;
    page_directory[33].MB_four.read_write = 1;
    page_directory[33].MB_four.user_supervisor = 1;
    page_directory[33].MB_four.write_through = 0;
    page_directory[33].MB_four.cache_disable = 0; 
    page_directory[33].MB_four.accessed = 0;
    page_directory[33].MB_four.dirty = 0;
    page_directory[33].MB_four.page_size = 1;                                                 // four MB config
    page_directory[33].MB_four.global = 0;                                                    // making it global
    page_directory[33].MB_four.available = 0;
    page_directory[33].MB_four.pat = 0;
    page_directory[33].MB_four.space = 0;    

    for (i = 0; i < num_entries; i++)
    {
        /* setting up the video memory at the specified location */
        if (i == VIDEO/FOUR_KB)
        {
            page_table[i].present = 1;                                                          // valid entry
            page_table[i].read_write = 1;                                                       // writable memory
            page_table[i].user_supervisor = 1;
            page_table[i].write_through = 0;
            page_table[i].cache_disable = 0; 
            page_table[i].accessed = 0;
            page_table[i].dirty = 0;
            page_table[i].pat = 0;
            page_table[i].global = 0;
            page_table[i].available = 0;
            page_table[i].addr_31_12 = i;
        }

        /* if not video memory,initialize it for unused memory */
        else
        {
            page_table[i].present = 0;
            page_table[i].read_write = 1;                                                       // writable memory
            page_table[i].user_supervisor = 0;
            page_table[i].write_through = 0;
            page_table[i].cache_disable = 0; 
            page_table[i].accessed = 0;
            page_table[i].dirty = 0;
            page_table[i].pat = 0;
            page_table[i].global = 0;
            page_table[i].available = 0;
            page_table[i].addr_31_12 = i;
        }
    }

    vidmap_table[0].present = 1;
    vidmap_table[0].read_write = 1;                                                       // writable memory
    vidmap_table[0].user_supervisor = 1;
    vidmap_table[0].write_through = 0;
    vidmap_table[0].cache_disable = 1;                                                    // cache disabled
    vidmap_table[0].accessed = 0;
    vidmap_table[0].dirty = 0;
    vidmap_table[0].pat = 0;
    vidmap_table[0].global = 0;
    vidmap_table[0].available = 0;
    vidmap_table[0].addr_31_12 = VIDEO >> shift_12;
    // initializing a new table for user video memory
    for (i = 1; i < num_entries; i++)
    {
        vidmap_table[i].present = 0;
        vidmap_table[i].read_write = 1;                                                       // writable memory
        vidmap_table[i].user_supervisor = 1;
        vidmap_table[i].write_through = 0;
        vidmap_table[i].cache_disable = 1;                                                    // cache disabled
        vidmap_table[i].accessed = 0;
        vidmap_table[i].dirty = 0;
        vidmap_table[i].pat = 0;
        vidmap_table[i].global = 0;
        vidmap_table[i].available = 0;
        vidmap_table[i].addr_31_12 = i;
    }

    page_table[VIDEO/FOUR_KB+1].present = 1;                                                          // valid entry
    page_table[VIDEO/FOUR_KB+1].read_write = 1;                                                       // writable memory
    page_table[VIDEO/FOUR_KB+1].user_supervisor = 1;
    page_table[VIDEO/FOUR_KB+1].write_through = 0;
    page_table[VIDEO/FOUR_KB+1].cache_disable = 0; 
    page_table[VIDEO/FOUR_KB+1].accessed = 0;
    page_table[VIDEO/FOUR_KB+1].dirty = 0;
    page_table[VIDEO/FOUR_KB+1].pat = 0;
    page_table[VIDEO/FOUR_KB+1].global = 0;
    page_table[VIDEO/FOUR_KB+1].available = 0;
    page_table[VIDEO/FOUR_KB+1].addr_31_12 = VIDEO/FOUR_KB+1;


    page_table[VIDEO/FOUR_KB+2].present = 1;                                                          // valid entry
    page_table[VIDEO/FOUR_KB+2].read_write = 1;                                                       // writable memory
    page_table[VIDEO/FOUR_KB+2].user_supervisor = 1;
    page_table[VIDEO/FOUR_KB+2].write_through = 0;
    page_table[VIDEO/FOUR_KB+2].cache_disable = 0; 
    page_table[VIDEO/FOUR_KB+2].accessed = 0;
    page_table[VIDEO/FOUR_KB+2].dirty = 0;
    page_table[VIDEO/FOUR_KB+2].pat = 0;
    page_table[VIDEO/FOUR_KB+2].global = 0; 
    page_table[VIDEO/FOUR_KB+2].available = 0;
    page_table[VIDEO/FOUR_KB+2].addr_31_12 = VIDEO/FOUR_KB+2;


    page_table[VIDEO/FOUR_KB+3].present = 1;                                                          // valid entry
    page_table[VIDEO/FOUR_KB+3].read_write = 1;                                                       // writable memory
    page_table[VIDEO/FOUR_KB+3].user_supervisor = 1;
    page_table[VIDEO/FOUR_KB+3].write_through = 0;
    page_table[VIDEO/FOUR_KB+3].cache_disable = 0; 
    page_table[VIDEO/FOUR_KB+3].accessed = 0;
    page_table[VIDEO/FOUR_KB+3].dirty = 0;
    page_table[VIDEO/FOUR_KB+3].pat = 0;
    page_table[VIDEO/FOUR_KB+3].global = 0;
    page_table[VIDEO/FOUR_KB+3].available = 0;
    page_table[VIDEO/FOUR_KB+3].addr_31_12 = VIDEO/FOUR_KB+3;


    /* control register setup for paging */
    loadPageDirectory((unsigned int* )page_directory);
}

/* new_program
 * 
 * Sets up the paging between virtual and physical memory 
 * Inputs: pid - process id number to help know which address we want to be using
 * Outputs: None
 * Side Effects: None
 */
void new_program(uint32_t pid){

    if (pid < 0 || pid >= 6)
        return -1;

    uint32_t addr = eightMB + (pid * fourMB); //Getting the address which will be shifted soon

    page_directory[32].MB_four.present = 1;
    page_directory[32].MB_four.user_supervisor = 1; 
    page_directory[32].MB_four.page_size = 1;
    page_directory[32].MB_four.addr_31_22 = (((unsigned int)(addr)) >> 22); //Getting the correct amoutn for 32 bit address
}


void update_vidmap(int terminal){   
    
    if (terminal == active_terminal) {
        vidmap_table[0].present = 1;
        vidmap_table[0].user_supervisor = 1;
        vidmap_table[0].addr_31_12 = VIDEO >> shift_12;

        page_table[VIDEO/FOUR_KB].present = 1;
        page_table[VIDEO/FOUR_KB].user_supervisor = 0;
        page_table[VIDEO/FOUR_KB].addr_31_12 = VIDEO >> shift_12;
    }

    else {
        vidmap_table[0].present = 1;
        vidmap_table[0].user_supervisor = 1;
        vidmap_table[0].addr_31_12 = VIDEO/FOUR_KB+1+terminal;

        page_table[VIDEO/FOUR_KB].present = 1;
        page_table[VIDEO/FOUR_KB].user_supervisor = 0;
        page_table[VIDEO/FOUR_KB].addr_31_12 = VIDEO/FOUR_KB+1+terminal;
    }

    return;
}