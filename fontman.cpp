// Implementation of the kernel's font management system

#include "fontman.h"
#include "kernel.h"
#include "vfs.h"

#include <stddef.h>
#include <stdbool.h>

#define MAX_FONTS 8

// This registry contains all fonts that are detected in the initrd
static font_t font_reg[MAX_FONTS];
static int loaded_fonts = 0;

// Points to the active font
static font_t* active_font = NULL;

/* These are private functions that help fontman run */
bool extension(const char* fname, const char ext)
{
    const char* f_ext = NULL;

    for (const char* p = fname; *p; ++p)
    {
        if (*p == '.')
        {
            f_ext = p;
        }
    }

    // If there is no extension
    if (ext == NULL)
    {
        // Returns false
        return false;
    }

    const char* e1 = f_ext;
    const char* e2 = ext;

    while (*e1 && (*e1 == *e2))
    {
        e1++;
        e2++;
    }
}

bool strcomp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 0;
}


// This scans for fonts, stored in the initrd, and populates the registry with any located fonts
void font_init()
{
    print_white("[INFO] Initializing font management\n");
    loaded_fonts = 0;

    // Look through every file in the VFS
    for (uint32_t i = 0; i < get_file_count(); ++i)
    {
        vfs_node node;
        f_stat_by_idx(i, &node);

        // Check for PSF files
        if (extension(node.name, ".psf") && loaded_fonts < MAX_FONTS)
        {
            font_t* new_font = &font_reg[loaded_fonts];

            // Copy the name of the font, minus the extension
        }
    
    }
}


// Unfinished