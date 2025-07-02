// The kernel :D


#include "gdt.h"
#include "kernel.h"
#include "idt.h"
#include "kbd.h"
#include "mem.h"
#include "sh.h"
#include "task.h"

#include <stdint.h>

// Next available memory address
static uint32_t free_memory_address = 0;

// Cursor position tracking
int cx = 0;
int cy = 0;

// Pointer to the VGA text mode buffer
volatile unsigned short* vmem = (unsigned short*)0xB8000;

// Initialize memory management
void mem_init(uint32_t kernel_end)
{
	// The memory should be free after the kernel ends
	free_memory_address = kernel_end;
}

uint32_t kmalloc(uint32_t size)
{
	uint32_t address = free_memory_address;
	// Bump up the pointer (Bump it up, BUMP IT)
	free_memory_address += size;

	return address;
}

// Updates the cursor on the screen
void update_cursor()
{
	uint16_t pos = cy * 80 + cx;

	ob(0x3D4, 0x0F);
	ob(0x3D5, (uint8_t)(pos & 0xFF));
	ob(0x3D4, 0x0E);
	ob(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}



void scroll()
{
	// While the cursor is not at the end of the screen
	if (cy < 25)
	{
		// Don't do anything
		return;
	}

	// Move each line up by one
	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 80; x++)
		{
			vmem[y * 80 + x] = vmem[(y + 1) * 80 + x];
		}
	}

	// Clear last line
	for (int x = 0; x < 80; x++)
	{
		vmem[24 * 80 + x] = (' ' | (0x07 << 8));
	}

	// Set cursor's Y position to the start of the next line
	cy = 24;
}


// Print a single character
void print_char(char c)
{
	// White on black
	unsigned char attr_byte = 0x07;
	unsigned short attr = attr_byte << 8;

	if (c == '\b')
	{
		if (cx > 0)
		{
			cx--;
			vmem[cy * 80 + cx] = ' ' | attr;
		}
	}

	// Deal with the weird ones
	else if (c == '\n')
	{
		cx = 0;
		cy++;
	}
	else if (c >= ' ')
	{
		vmem[cy * 80 + cx] = c | attr;
		cx++;
	}

	// Go to the next line if this line is full
	if (cx >= 80)
	{
		cx = 0;
		cy++;
	}

	// Scroll if needed
	scroll();
	update_cursor();
}


// This stuff is available to the rest of the kernel as well

// Clear the screen
void clear()
{
	unsigned char attr_byte = 0x07;
	unsigned short blank = ' ' | (attr_byte << 8);

	for (int i = 0; i < 80 * 25; i++)
	{
		vmem[i] = blank;
	}

	cx = 0;
	cy = 0;
}


// Print a string, ending with a null terminator
void print(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		print_char(str[i]);
	}
}


// The kernel's entry point
extern "C" void kmain()
{
	extern uint32_t end;

	// Clear the screen
	clear();
	// Welcome message
	print("[INFO] KERNEL LOADED SUCCESSFULLY.\n");

	// Initialize GDT
	gdt_init();
	// Initialize the IDT
	idt_init();
	// Initialize the keyboard driver
	kbd_init();
	// Initialize multitasking
	task_init();
	// Initialize the shell
	sh_init();

	// Enable interrupts
	asm volatile("sti");

	// Start shell
	sh_loop();

	// If you get here, brokey
	print("FATAL ERROR: SHELL EXITED UNEXPECTEDLY. BIG DEALIO.\n");


	// Halt the CPU
	while (true)
	{

	}
}
