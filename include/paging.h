// Header file for paging

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

extern uint32_t* kernel_dir;

// Initialize paging
void paging_init();

// Switch page directory
void paging_cd(uint32_t* dir);

// Allocate a new page directory for a given process
uint32_t* paging_mkdir();

// Map a page, using appropriate flags for the user/kernel
void map_page(uint32_t* dir, uint32_t vaddr, uint32_t paddr, uint32_t flags);


#endif