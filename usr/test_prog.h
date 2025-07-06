// A test program
#ifndef TEST_PROG_H
#define TEST_PROG_H

#include <stdint.h>

// This just invokes a system call. Wowza
static uint8_t test_prog[] =
{
    0xB8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
    0xCD, 0x80                    // int 0x80 (syscall)
};


#define PROG_SIZE (sizeof(test_prog))
#define PROG_ENTRY 0x400000

#endif