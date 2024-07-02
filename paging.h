#ifndef PAGING_H_
#define PAGING_H_

#include "x86_desc.h"
#include "lib.h"
#include "paging_helper.h"
#include "types.h"

#define FOUR_KB                         4096
#define num_entries                     1024
#define shift_12                        12

/* initializations for */
PDE_t page_directory[num_entries] __attribute__((aligned(FOUR_KB)));
struct PTE_4KB_t page_table[num_entries] __attribute__((aligned(FOUR_KB)));
struct PTE_4KB_t vidmap_table[num_entries] __attribute__((aligned(FOUR_KB)));

extern void page_init();
extern void new_program(uint32_t pid);
extern void update_vidmap(int terminal);

#endif 
