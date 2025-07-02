// Shell

#include "kbd.h"
#include "kernel.h"
#include "sh.h"

#include <stddef.h>
#include <stdbool.h>


#define MAX_CMD_LENGTH 256

// String comparison
bool strcomp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 0;
}

// Handle commands
void handle_cmd(const char* cmd)
{
    if (strcomp(cmd, "clear")) {
        clear();
    } else if (strcomp(cmd, "help")) {
        print("\nAVAILABLE COMMANDS\n");
        print("clear - Clear the screen\n");
        print("help - Show this help message\n");
        print("exit - Exit the shell\n");
    } else {
        print("\nCommand not found: ");
        print(cmd);
        print("\n");
    }
}

// Initialize the shell
void sh_init()
{
    print("Welcome to Kurios32\n");
    print("Type 'help' for a list of commands\n");
}

// The shell's main loop
void sh_loop()
{
    char cmd_buffer[MAX_CMD_LENGTH];
    size_t buff_pos = 0;

    print("kurios32> ");

    while (true)
    {
        char c = getchar(); // Get character from keyboard

        if (c == '\n' || c == '\r')
        {
            if (buff_pos > 0)
            {
                cmd_buffer[buff_pos] = '\0';
                handle_cmd(cmd_buffer);
                buff_pos = 0;
            }
            print("\nkurios32> ");
        }
        // Backspace
        else if (c == '\b')
        {
            // If it isn't the first character
            if (buff_pos > 0)
            {
                // Move the buffer back by one
                buff_pos--;
                // Moves the cursor back
                print_char('\b');
                // Write a blank character (overwrites the existing character)
                print_char(' ');
                // Move the cursor back again
                print_char('\b');
            }
        }
        else if (buff_pos < MAX_CMD_LENGTH - 1)
        {
            cmd_buffer[buff_pos++] = c;
            print_char(c);
        }
    }
}