// Header file for paging

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// These are flags for pages

// Page is presently in the memory
#define PAGE_PRESENT 0x1

// Page is read/write
#define PAGE_WRITE   0x2

// Page can be accessed via usermode
#define PAGE_USER    0x4

#define PAGE_FRAME_ADDRESS 0xFFFFF000


// Page table with 1024, 4-byte entries (4KB)
typedef struct
{
    uint32_t entries[1024];
} page_tab_t;

// Page directory with 1024 pointers to page tables (4KB)
typedef struct
{
    uint32_t tables[1024];
    uint32_t tables_physical;
} page_dir_t;


void paging_init();
void paging_cd(page_dir_t* dir);
page_dir_t* page_cldir(page_dir_t* src);
void map_page(page_dir_t* dir, uint32_t vaddr, uint32_t paddr, uint32_t flags);
page_dir_t* get_dir();


#endif