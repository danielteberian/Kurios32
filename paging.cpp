// Paging for the kernel

#include "include/error.h"
#include "include/global.h"
#include "include/kernel.h"
#include "include/mem.h"
#include "include/paging.h"
#include "include/serial.h"
#include "include/task.h"


static page_dir_t k_dir_storage __attribute__((aligned(4096)));
// Master page directory
page_dir_t* k_dir = 0;
// Active page directory
page_dir_t* active_dir = 0;


// Page fault handler
extern "C" void page_fault_c_handler(reg_t* regs, uint32_t errc)
{
    uint32_t faddr;
    asm volatile("mov %%cr2, %0" : "=r"(faddr));
    serial_print("\n[!!!] KERNEL PANIC. PAGE FAULT\N");

    char buffer;

    print_red("\n [ERR] PAGE FAULT: ");
    print_hex(faddr);
    print("\n [ERR] Error code: ");
    print_hex(errc);

    if (!(errc & 0x1))
    {
        serial_print("\nPage not present.\n");
    }

    if (errc & 0x2)
    {
        serial_print("\nPage is read-only, but an attempt was made to write to it.\n");
    }

    if (errc & 0x4)
    {
        serial_print("\nPage is in usermode, but the kernel tried to access it.\n");
    }

    if (errc & 0x8)
    {
        serial_print("\nAn attempt was made to overwrite reserved bits.\n");
    }

    if (errc & 0x10)
    {
        serial_print("\nInstruction fetching error.\n");
    }

    // TODO: Print register statuses


    punchout("The system was halted due to a page fault.");
}


void paging_init()
{

    // Allocate a frame from the kernel's page directory
    uint32_t pdir = pmm_alloc_frame();
    k_dir = (page_dir_t*)pdir;

    // Clear the directory
    for (int i = 0; i < 1024; i++)
    {
        k_dir -> tables[i] = 0 | PAGE_WRITE;
    }

    k_dir -> tables_physical = pdir;

    // Create page tables, map kernel in higher half
    extern uint32_t end;
    uint32_t k_end_addr = (uint32_t)&end;

    for (uint32_t vaddr = 0xC0000000; vaddr < 0xC0000000 + k_end_addr; vaddr += 0x1000)
    {
        uint32_t paddr = vaddr - 0xC0000000;
        map_page(k_dir, vaddr, paddr, PAGE_PRESENT | PAGE_WRITE);
    }

    // ID map first 4MB for HW access
    for (uint32_t addr = 0; addr < 0x400000; addr += 0x1000)
    {
        map_page(k_dir, addr, addr, PAGE_PRESENT | PAGE_WRITE);
    }

    // Switch to new page directory
    paging_cd(k_dir);

    log(LOG_INFO, "Paging initialized.\n");
}


// Change paging dir
void paging_cd(page_dir_t* dir)
{
    active_dir = dir;

    asm volatile(
        "mov %0, %%cr3"
        :: "r"(dir -> tables_physical)
    );
}


// Clone paging directory
page_dir_t* page_cldir(page_dir_t* src)
{
    uint32_t pdir = pmm_alloc_frame();
    page_dir_t* new_dir = (page_dir_t*)pdir;

    for (int i = 0; i < 1024; i++)
    {
        new_dir -> tables[i] = 0 | PAGE_WRITE;
    }

    new_dir -> tables_physical = pdir;


    for (int i = 768; i < 1024; i++)
    {
        if ((uint32_t)src -> tables[i] & PAGE_PRESENT)
        {
            new_dir -> tables[i] = src -> tables[i];
        }
    }

    return new_dir;
}


void map_page(page_dir_t* dir, uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
    uint32_t didx = vaddr >> 22;
    uint32_t tidx = (vaddr >> 12) & 0x3FF;

    if (!(dir -> tables[didx] & PAGE_PRESENT))
    {
        uint32_t ptab = pmm_alloc_frame();
        page_tab_t* new_tab = (page_tab_t*)ptab;

        for (int i = 0; i < 1024; i++)
        {
            new_tab -> entries[i] = 0 | PAGE_WRITE;
        }

        dir -> tables[didx] = ptab | flags | PAGE_PRESENT;
    }

    page_tab_t* tab = (page_tab_t*)((uint32_t)dir -> tables[didx] & PAGE_FRAME_ADDRESS);
    tab -> entries[tidx] = (paddr & PAGE_FRAME_ADDRESS) | flags | PAGE_PRESENT;
}


page_dir_t* get_dir()
{
    return active_dir;
}
