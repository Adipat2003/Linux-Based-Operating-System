#ifndef _PAGING_HELPER_H
#define _PAGING_HELPER_H

/* loadPageDirectory
 * 
 * Inspired from wikidev
 * Stores the pointer to the array of page directory
 * structs into cr3 and initializes paging for 4MB and 
 * 4KB memories (using cr0 and cr4).
 * Inputs: None
 * Outputs: None
 * Side Effects: divides up the memory into respective pages
 */
extern void loadPageDirectory(unsigned int*); // defining the function
extern void flush_tlb();

#endif
