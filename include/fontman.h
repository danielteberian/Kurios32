// The header file for the kernel's font management system

#ifndef FONTMAN_H
#define FONTMAN_H

#include <stdint.h>


// The structure used for a bitmap font
typedef struct font_t
{
    // The name of the font
    char name[32];
    // The width of a single character, in pixels
    uint8_t cw;
    // The height of a single character, in pixels
    uint8_t ch;
    // The filesize of the font, in bytes
    uint32_t filesize;
    // Pointer to the font, loaded from VFS
    unsigned char* data;
};

// Initialize font management, scan for fonts
void fontman_init();

// Sets the font
// True if font is set, successfully.
// False if font is not found.
bool font_set(const char* font_name);

// Return pointer to currently active font
font_t* font_active();

// List available fonts
void font_list();

#endif