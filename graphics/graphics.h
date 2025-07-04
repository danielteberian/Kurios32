// Basic graphics support

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>
#include "../multiboot.h"

// 32-bit color
typedef uint32_t color_t;

// Initalize graphics driver, based on information from Multiboot
bool graphics_init(mb_info_t* mbt);

// Draw a pixel at a given position
void draw_pixel(int x, int y, color_t color);

// Fill a rectangular angle with a color
void draw_rect_full(int x, int y, int w, int h, color_t color);

// Clear the screen and fill with a specific color
void graphics_clear(color_t color);

// Create 32-bit color from given RGB values
static inline color_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) | (g << 8) | b;
}

#endif