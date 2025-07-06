// Loads a program, deals with paging, and maps the code/stack

#include "../mem.h"
#include "../paging.h"
#include "test_prog.h"

#define U_STACK_TOP 0xBFFFF000


// This should return the user page directory
uint32_t* uload(uint32_t* u_eip, uint32_t* u_esp)
{
    // Create page directory
    uint32_t* u_dir = paging_mkdir();

    // Allocate physical memory for code
    uint32_t code_p = kmalloc(PROG_SIZE);
    // Allocate physical memory for stack
    uint32_t stack_p1 = kmalloc(4096);
    uint32_t stack_p2 = kmalloc(4096);
}