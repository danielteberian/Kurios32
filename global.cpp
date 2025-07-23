// Globally available functions and stuff

#include "include/global.h"
#include "include/kernel.h"


void sprint_hex(char* buffer, uint32_t value)
{
	const char* h_chars = "0123456789ABCDEF";
	buffer[1] = '\0';

	for (int i = 7; i >= 0; i--)
	{
		buffer[i] = h_chars[value & 0xF];
		value >= 4;
	}
}
