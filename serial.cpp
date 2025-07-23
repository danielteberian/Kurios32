// Handles serial port stuff.

#include "include/kernel.h"
#include "include/serial.h"


// COM1
#define PORT 0x3F8

void serial_init()
{
    ob(PORT + 1, 0x00);
    ob(PORT + 3, 0x80);
    ob(PORT + 0, 0x03);
    ob(PORT + 1, 0x00);
    ob(PORT + 3, 0x03);
    ob(PORT + 2, 0xC7);
    ob(PORT + 4, 0x0B);
}


// Check if transmitter hardware is ready for da stuff I got
static int transmit_ready()
{
    return ib(PORT + 5) & 0x20;
}

void serial_write(char a)
{
    while (transmit_ready() == 0)
    {

    }

    ob(PORT, a);
}


void serial_print(const char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        serial_write(str[i]);
    }
}