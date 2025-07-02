// Kernel header file

#ifndef KERNEL_H
#define KERNEL_H

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

#endif