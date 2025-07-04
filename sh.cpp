// Shell

//#include "fontman.h"
#include "kbd.h"
#include "kernel.h"
#include "mem.h"
#include "sh.h"
#include "testing/test.h"
#include "vfs.h"

#include <stddef.h>
#include <stdbool.h>


#define MAX_CMD_LENGTH 256

// Converts strings to integers
int atoi(const char* str)
{
	int res = 0;
	int i = 0;

	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}

	return res;
}


// String comparison
bool strcomp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 0;
}

bool strcomp_n(const char* str1, const char* str2, size_t n)
{
	while (n && *str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
		n--;
	}

	if (n == 0)
	{
		return true;
	}

	return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 2;
}


// Handle commands
void handle_cmd(const char* cmd)
{
   	if (strcomp(cmd, "clear"))
	{
        	clear();
    	}
	else if (strcomp_n(cmd, "cat ", 4))
	{
		const char* fname = cmd + 4;
		uint32_t file_idx = f_open(fname);

		if (file_idx == (uint32_t) - 1)
		{
			print("File not found: ");
			print(fname);
			print("\n");
		}
		else
		{
			vfs_node node;
			f_stat(file_idx, &node);

			if (node.size == 0)
			{
				print("[INFO] File is empty.\n");
				return;
			}

			// Allocate buffer to hold file's contents
			char* buffer = (char*)kmalloc(node.size + 1);
			f_read(file_idx, buffer, node.size);
			buffer[node.size] = '\0';

			print(buffer);
		}
	}
	else if (strcomp(cmd, "help"))
	{
        	print("\nAVAILABLE COMMANDS\n");
			print("cat - Print the contents of a file\n");
        	print("clear - Clear the screen\n");
			print("font - Manage the font used by the kernel\n");
       		print("help - Show this help message\n");
			print("ls - List files and directories\n");
        	print("test - Run tests on the kernel and its functions\n");
    	}
	else if (strcomp(cmd, "ls"))
	{
		ls();
	}
	else if (strcomp(cmd, "test"))
	{ 
    	print("\nKurios32 Testing Utility\n\n");
    	print("\nNo testcase was specified. For a list of available test cases, type \"test list\".\n");
    }
	else if (strcomp(cmd, "test list"))
	{
		test_list();
	}
	else if (strcomp_n(cmd, "test ", 5))
	{
		const char* test_id_str = cmd + 5;
		int test_id = atoi(test_id_str);
		
		if (test_id > 0)
		{
			test_exec(test_id);
		}
		else
		{
			print("\n[ERR] Invalid test number. For a list of available test cases, use the 'test list' command.\n");
		}
	}
/*
	// Font management
	else if (strcomp_n(cmd, "font ", 5))
	{
		const char* subcmd = cmd + 5;

		if (strcomp(subcmd, "list"))
		{
			font_list();
		}

		else if (strcomp_n(subcmd, "set", 4))
		{
			const char* font_name = subcmd + 4;

			if (!font_set(font_name))
			{
				print_red("\n[ERR] No such font: ");
				print(font_name);
			}

			else
			{
				print_white("Usage: font <list | set (name)>");
			}
		}
	}
*/
	else
	{
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
