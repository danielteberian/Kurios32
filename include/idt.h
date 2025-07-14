// Interrupt Descriptor Table header

#ifndef IDT_H
#define IDT_H


// Libraries to include
#include <stddef.h>
#include <stdint.h>


// Used to define an entry in the IDT
struct idt_item
{
	uint16_t base_lo;
	uint16_t sel;
	uint8_t always0;
	uint8_t flags;
	uint16_t base_hi;
}

__attribute__((packed));


// Define a pointer to the IDT
struct idt_ptr
{
	uint16_t lim;
	uint32_t base;
}

__attribute__((packed));


// Initialize IDT
void idt_init();

#endif
