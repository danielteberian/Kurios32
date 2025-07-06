// This header has declarations for string utilities

#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stdint.h>


// Convert ASCII to integer
int atoi(const char* str);

// Compare two strings
bool strcomp(const char* str1, const char* str2);

#endif
