#ifndef MEM_H
#define MEM_H

#include <stdint.h>

void mem_init(uint32_t kernel_end);

uint32_t kmalloc(uint32_t size);
uint32_t kmalloc_a(uint32_t size);

#endif