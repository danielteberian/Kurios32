// Keyboard driver

#include "kbd.h"
#include "kernel.h"


#define KBD_DATA 0x60
#define KBD_STAT 0x64
#define KBD_BUFF 256

// Keyboard buffer
static char kbd_buff[KBD_BUFF];
// Buffer read index
static int buff_ridx = 0;
// Buffer write index
static int buff_widx = 0;


// Scancodes to ASCII (US)
const char scancode_to_ascii[] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


// This is called by the keyboard interrupt handler
extern "C" void kbd_handler_main()
{
    unsigned char status;
    unsigned char scancode;

    // Read the status from the keyboard controller
    status = ib(KBD_STAT);

    // Check if there is data available
    if (status & 0x01)
    {
        scancode = ib(KBD_DATA);

        // Ignore keyboard releases (scancode is greater than 0x80)
        if (scancode > 0 && scancode < sizeof(scancode_to_ascii))
        {
            char ascii_char = scancode_to_ascii[(int)scancode];

            if (ascii_char != 0)
            {
                int next_idx = (buff_widx + 1) % KBD_BUFF;

                if (next_idx != buff_ridx)
                {
                    kbd_buff[buff_widx] = ascii_char;
                    buff_widx = next_idx;
                }
            }
        }
    }

    // Signal end of interrupt to the PIC
    ob(0x20, 0x20);
}

// Initialize keyboard driver
void kbd_init()
{
    // This is done via assembly. Prints a message anyway
    print("[INFO] Keyboard driver initialized\n");
}

// Read a character from the keyboard buffer
char getchar()
{
    // Await an available character
    while (buff_ridx == buff_widx)
    {
        // Halt CPU until the next interrupt
        asm volatile("hlt");
    }

    char c = kbd_buff[buff_ridx];
    buff_ridx = (buff_ridx + 1) % KBD_BUFF;
    return c;
}