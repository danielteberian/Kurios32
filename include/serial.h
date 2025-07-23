// A simple header to help the kernel handle serial input. This is for debugging.

#ifndef SERIAL_H
#define SERIAL_H

void serial_init();
void serial_print(const char* str);
void serial_write(char a);


#endif