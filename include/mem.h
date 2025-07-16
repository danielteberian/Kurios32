// This header file is designed to provide the kernel with memory management functions.

#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include "include/multiboot.h"

// The size of the physical memory manager's frame
#define PMM_F_SIZE 4096
// How many frames per byte
#define PMM_FPB 8

// Initialize the PMM
void pmm_init(mb_info_t* mbt);

// Allocate a frame
uint32_t pmm_alloc_frame();

// Free a PMM frame
void pmm_free(uint32_t f_addr);

// Initialize kernel heap
void kmem_init();

// Allocate block of memory of a given size
void* kmalloc(uint32_t size);

// Free a block of memory that had been allocated by kmalloc
void kfree(void* ptr);

#endif