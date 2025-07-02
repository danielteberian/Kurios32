// Global Descriptor Table (GDT) implementation

#include "gdt.h"
#include "kernel.h"


#define GDT_ITEMS 6


struct gdt_item gdt[GDT_ITEMS];
struct gdt_ptr gdt_p;
struct tss_item tss;

// Use the assembly from gdt.asm to load the new GDT and TSS
extern "C" void flush_gdt(uint32_t gdt_ptr_addr);
extern "C" void flush_tss();


// This is a function that helps create a GDT entry
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_mid = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].gran = (limit >> 16) & 0x0F;

	gdt[num].gran |= granularity & 0xF0;
	gdt[num].access = access;
}


// This is a function that helps create an entry in the TSS
void write_tss(int32_t num, uint16_t ss0, uint32_t esp0)
{
	uint32_t base = (uint32_t)&tss;
	uint32_t limit = base + sizeof(tss);

	gdt_set_gate(num, base, limit, 0xE9, 0x00);

	// Clear out the TSS
	for (int i = 0; i < sizeof(tss); ++i)
	{
		((char*)&tss)[i] = 0;
	}

	tss.ss0 = ss0;
	tss.esp0 = esp0;
	tss.cs = 0x0b;
	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
}


// Initialize GDT
void gdt_init()
{
	gdt_p.limit = (sizeof(struct gdt_item) * GDT_ITEMS) - 1;
	gdt_p.base = (uint32_t)&gdt;

	// NULL descriptor
	gdt_set_gate(0, 0, 0, 0, 0);
	// Ring 0, kernel code segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	// Ring 0, kernel data segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	// Ring 3, user code segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	// Ring 3, user data segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	// TSS segment
	// Kernel data segment selector, ESP0 is set to 0
	write_tss(5, 0x10, 0);

	// Load GDT
	flush_gdt((uint32_t)&gdt_p);
	// Print a message
	print("\n[INFO] GDT INITIALIZED.\n");

	// Load TSS
	flush_tss();
	// Print message
	print("\n[INFO] TSS INITIALIZED.\n");
}
