// Implementation of paging for the kernel

#include "include/mem.h"
#include "include/paging.h"


#define PAGE_ITEMS 1024
#define PAGE_SIZE 4096


// Page directory for the kernel
uint32_t* kernel_dir = (uint32_t*)0; // Initialize to null

// Initialize paging
void paging_init()
{
    // Allocate and align the kernel directory
    kernel_dir = (uint32_t*)kmalloc_a(PAGE_ITEMS * sizeof(uint32_t));

    // First, prepare for the kernel/heap by ID mapping the first 4 MB
    static uint32_t first_tab[PAGE_ITEMS] __attribute__((aligned(PAGE_SIZE)));

    for (uint32_t i = 0; i < PAGE_ITEMS; i++)
    {
        first_tab[i] = (i * PAGE_SIZE) | 3;
    }

    kernel_dir[0] = ((uint32_t)first_tab) | 3;

    for (uint32_t i = 1; i < PAGE_ITEMS; i++)
    {
        kernel_dir[i] = 0;
    }

    // Load page directory, enable paging
    asm volatile
    (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        : "r"(kernel_dir)
        : "eax"
    );
}


// Switch page directory
void paging_cd(uint32_t* dir)
{
    asm volatile
    (
        "mov %0, %%cr3" :: "r"(dir)
    );
}


// Create a new paging directory
uint32_t* paging_mkdir()
{
    uint32_t* new_dir = (uint32_t*)kmalloc(PAGE_ITEMS * sizeof(uint32_t));

    if (!new_dir)
    {
        return nullptr;
    }

    // Copy kernel mappings
    for (uint32_t i = 768; i < PAGE_ITEMS; i++)
    {
        new_dir[i] = kernel_dir[i];
    }

    // Zero-out userspace
    for (uint32_t i = 0; i < 768; i++)
    {
        new_dir[i] = 0;
    }

    // Viola
    return new_dir;
}


// Maps a page with appropriate flags for the user/kernel
void map_page(uint32_t* dir, uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
    uint32_t dir_idx = vaddr >> 22;
    uint32_t tab_idx = (vaddr >> 12) & 0x3FF;


    // When needed, allocate a page table
    if (!(dir[dir_idx] & 1))
    {
        uint32_t* new_tab = (uint32_t*)kmalloc(PAGE_ITEMS * sizeof(uint32_t));

        for (uint32_t i = 0; i < PAGE_ITEMS; i++)
        {
            new_tab[i] = 0;
        }

        // Set the user/supervisor bit in the page directory's entry
        dir[dir_idx] = ((uint32_t)new_tab) | (flags & 0x4) | 3;
    }

    uint32_t* tab = (uint32_t*)(dir[dir_idx] & 0xFFFFF000);
    tab[tab_idx] = (paddr & 0xFFFFF000) | (flags & 0x7) | 1;
}
