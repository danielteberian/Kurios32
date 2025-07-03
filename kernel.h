// Kernel header file

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>


// Print hexadecimal
void print_hex(uint32_t n);

// Print decimal
void print_dec(uint32_t n);

// Print string
void print(const char* str);

// Print character
void print_char(char c);

// Clear screen, reset cursor position
void clear();

// Write a byte to given I/O port
static inline void ob(unsigned short port, unsigned char data)
{
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}


// Read a byte from given I/O port
static inline unsigned char ib(unsigned short port)
{
    unsigned char data;
    asm volatile ("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// This is used to change the color of text
// Unimplemented
enum vga_color
{
	C_BLACK = 0,
	C_BLUE = 1,
	C_GREEN = 2,
	C_CYAN = 3,
	C_RED = 4,
	C_MAGENTA = 5,
	C_BROWN = 6,
	C_LIGHT_GREY = 7,
	C_DARK_GREY = 8,
	C_LIGHT_BLUE = 9,
	C_DARK_BLUE = 10,
	C_LIGHT_CYAN = 11,
	C_LIGHT_RED = 12,
	C_LIGHT_MAGENTA = 13,
	C_YELLOW = 14,
	C_WHITE = 15,
};







#endif
