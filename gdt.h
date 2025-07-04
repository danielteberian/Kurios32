// Global Descriptor Table (GDT) header

#ifndef GDT_H
#define GDT_H

#include <stdint.h>


// Defines a GDT entry
struct gdt_item
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t gran;
	uint8_t base_high;
}
__attribute__((packed));


// Defines a pointer to GDT
struct gdt_ptr
{
	uint16_t limit;
	uint32_t base;
}
__attribute__((packed));


// Structure for the TSS
struct tss_item
{
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
}
__attribute__((packed));


// Initialize GDT
void gdt_init();


#endif
