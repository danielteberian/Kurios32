// Implementation of the basic graphics driver

#include "graphics.h"
#include "../kernel.h"


// Display properties

// Framebuffer
static uint32_t* fb;
// Screen width
static uint32_t sw;
// Screen height
static uint32_t sh;
// Screen pitch
static uint32_t sp;
// Bits per pixel
static uint8_t bpp;


// Initialize graphics driver
bool graphics_init(mb_info_t* mbt)
{
    // See if the code for the multiboot stuff actually worked
    if (!(mbt->flags & MULTIBOOT_INFO_FRAMEBUFFER))
    {
        print("\n[INFO] Bootloader did not provide info about the framebuffer.\n");
        return false;
    }

    fb =(uint32_t*)((uintptr_t)mbt->framebuffer_addr);
    sw = mbt->framebuffer_width;
    sh = mbt->framebuffer_height;
    sp = mbt->framebuffer_pitch;
    bpp = mbt->framebuffer_bpp;

    if (fb == 0 || bpp != 32)
    {
        print_red("\n[ERR] Your graphics mode is unsupported.\n");
        return false;
    }

    print_green("\n[INFO] Graphics mode has been initialized.\n");
    print_dec(sw);
    print_white("X");
    print_dec(sh);
    print_white("X");
    print_dec(bpp);
    print_white("bpp\n");

    return true;
}

// Draw a pixel at a given position
void draw_pixel(int x, int y, color_t color)
{
    // Make sure it fits on the screen
    if (x >= 0 && (uint32_t)x < sw && y >= 0 && (uint32_t)y < sh)
    {
        // Find linear address for the pixel
        uint32_t* pa = (uint32_t*)((uint8_t*)fb + y * sp + x * (bpp / 8));
        *pa = color; 
    }
}


// Draw a rectangle that is a solid color
void draw_rect_full(int x, int y, int w, int h, color_t color)
{
    for (int j = y; j < y + h; ++j)
    {
        for (int i = x; i < x + w; ++i)
        {
            draw_pixel(i, j, color);
        }
    }
}

// Make the whole screen a given color
void graphics_clear(color_t color)
{
    // Number of pixels
    uint32_t np = sw * sh;

    for (uint32_t i = 0; i < np; ++i)
    {
        fb[i] = color;
    }
}