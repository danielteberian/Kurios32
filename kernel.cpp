// The kernel :D

//#include "fontman.h"
#include "gdt.h"
#include "kernel.h"
#include "idt.h"
#include "kbd.h"
#include "mem.h"
#include "multiboot.h"
#include "sh.h"
#include "task.h"
#include "vfs.h"

#include "graphics/graphics.h"
#include "testing/test.h"

#include <stdint.h>


// Whether or not graphics are enabled
static bool graphics_mode = false;

// Next available memory address
static uint32_t free_memory_address = 0;

// Cursor position tracking
int cx = 0;
int cy = 0;

// This is the default color to use for text
const uint8_t DEFAULT_COLOR = C_LIGHT_GREY | (C_BLACK << 4);

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


// Print decimal
void print_dec(uint32_t n)
{
	if (n == 0)
	{
		print_char('0');
		return;
	}

	char buff[11];
	int i = 9;
	buff[10] = '\0';

	while (n > 0)
	{
		buff[i--] = (n % 10) + '0';
		n /= 10;
	}

	print(buff + i + 1);
}


// Print hexadecimal
void print_hex(uint32_t n)
{
	char buff[11] = "0x";
	char* hex = "0123456789ABCDEF";
	int i = 9;

	if (n == 0)
	{
		buff[2] = '0';
		buff[3] = '\0';
	}
	else
	{
		while (n != 0)
		{
			buff[i--] = hex[n % 16];
			n /= 16;
		}
	}

	buff[10] = '\0';
	print(buff + i + 1);
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
	// If graphics are enabled
	if (graphics_mode)
	{
		graphics_clear(rgb(0, 0, 0));
	}
	else
	{
		unsigned char attr_byte = 0x07;
		unsigned short blank = ' ' | (attr_byte << 8);

		for (int i = 0; i < 80 * 25; i++)
		{
			vmem[i] = blank;
		}
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

// Print a single character with color
void print_char_color(char c, uint8_t color_attr)
{
	unsigned short attr = color_attr << 8;

	if (c == '\b')
	{
		if (cx > 0)
		{
			cx--;
			vmem[cy * 80 + cx] = ' ' | attr;
		}
	}

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

	if (cx >= 80)
	{
		cx = 0;
		cy++;
	}

	scroll();
	update_cursor();
}

// Print a string of colored text
void print_color(const char* str, uint8_t color)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		print_char_color(str[i], color);
	}
}

// Add a VGA color attribute byte
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}


// Print red text
void print_red(const char* str)
{
    print_color(str, vga_entry_color(C_RED, C_BLACK));
}

// Print green text
void print_green(const char* str)
{
	print_color(str, vga_entry_color(C_GREEN, C_BLACK));
}

// Print blue text
void print_blue(const char* str)
{
	print_color(str, vga_entry_color(C_BLUE, C_BLACK));
}

// Print cyan text
void print_cyan(const char* str)
{
	print_color(str, vga_entry_color(C_CYAN, C_BLACK));
}

// Print magenta text
void print_magenta(const char* str)
{
	print_color(str, vga_entry_color(C_MAGENTA, C_BLACK));
}

// Print white text
void print_white(const char* str)
{
	print_color(str, vga_entry_color(C_WHITE, C_BLACK));
}

// The kernel's entry point
extern "C" void kmain(mb_info_t* mbt, uint32_t magic)
{
	extern uint32_t end;

	graphics_mode = graphics_init(mbt);

	// Clear the screen
	clear();

	mem_init((uint32_t)&end);

	print_green("[INFO] Filesystem initialized.\n");

	// A test for the graphics
	if (graphics_mode)
	{
		draw_rect_full(100, 100, 200, 150, rgb(255, 0, 0));
		draw_rect_full(150, 150, 200, 150, rgb(0, 255, 0));
		draw_rect_full(200, 200, 200, 150, rgb(0, 0, 255));
	}

	if (magic != 0x2BADB002)
	{
		print("[FATAL ERROR] Invalid multiboot magic number.\n");
		return;
	}

	// Welcome message
	print("[INFO] KERNEL LOADED SUCCESSFULLY.\n");

	// Initialize GDT
	gdt_init();
	// Initialize memory management
	mem_init((uint32_t)&end);
	// Initialize VFS
	vfs_init(mbt);
	// Initialize font-management system
	// fontman_init();
	// Initialize the IDT
	idt_init();
	// Initialize the keyboard driver
	kbd_init();
	// Initialize multitasking
	task_init();
	// Initialize the testing framework
	test_init();
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
