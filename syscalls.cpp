// Syscall handling for the kernel

#include "kernel.h"

extern "C" void syscall_c_handler()
{
    print("\n[INFO] A system call has been invoked.\n");
}