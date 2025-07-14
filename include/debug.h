// Header file for the kernel's integrated debugger

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

// Debug messages
void dlog(int lvl, const char* msg);

// Dump memory
void memdump(uint32_t addr, uint32_t count);

// Debug command integration for the shell
void dregsh();

#endif