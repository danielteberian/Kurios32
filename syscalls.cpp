// Syscall handling for the kernel

#include "kernel.h"
#include "sh.h"
#include "paging.h"

extern uint32_t* kernel_dir;

extern "C" void syscall_c_handler()
{
    uint32_t syscall_id;

    asm volatile("movl %%eax, %0" : "=r"(syscall_id));

    if (syscall_id == 1)
    {
        print_cyan("\n[INFO] System call exit has been invoked. returning to kernel mode.\n");
        sh_loop();

        while(1)
        {
            asm volatile("hlt");
        }
    }
    else
    {
        print_cyan("\n[INFO] A system call has been invoked.\n");
    }
}