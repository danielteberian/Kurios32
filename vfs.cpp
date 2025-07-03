// A basic VFS implementation for the kernel

#include "kernel.h"
#include "vfs.h"


// This array stores information about the files in the initrd
static vfs_node file_tab[MAX_FILES];
static uint32_t file_count = 0;

// This is a simple method for comparing strings
bool strcmp(const char* str1, const char* str2)
{
	while (*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}

	return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 0;
}


// File statistics
void f_stat(uint32_t node_idx, vfs_node* node_out)
{
	if (node_idx < file_count && node_out != 0)
	{
		*node_out = file_tab[node_idx];
	}
}


// Convert octal to decimal
uint32_t oct_to_dec(const char *oct)
{
	uint32_t dec = 0;
	int i = 0;

	while (oct[i] && (oct[i] >= '0' && oct[i] <= '7'))
	{
		dec = dec * 8 + (oct[i] - '0');
		i++;
	}

	return dec;
}


// Initialize VFS
void vfs_init(mb_info_t* mbt)
{
	if (!(mbt->flags & MULTIBOOT_INFO_MODS))
	{
		print("[INFO] NO INITRD MODULE COULD BE FOUND\n");
		return;
	}

	mb_mod_t* mod = (mb_mod_t*)mbt->mods_addr;
	uint32_t initrd_addr = mod->mod_start;
	uint32_t initrd_end = mod->mod_end;

	print("[INFO] INITRD LOCATED AT 0x\n");
	print_hex(initrd_addr);
	print(", End: 0x");
	print_hex(initrd_end);
	print("\n");

	uint32_t addr = initrd_addr;

	while (addr < initrd_end && file_count < MAX_FILES)
	{
		char* header = (char*)addr;

		if (header[0] == '\0')
		{
			break;
		}

		char size_oct[12];

		for (int i=0; i < 11; ++i)
		{
			size_oct[i] = header[124 + i];
		}

		size_oct[11] = '\0';
		uint32_t size = oct_to_dec(size_oct);

		for (int i = 0; i < 100; ++i)
		{
			file_tab[file_count].name[i] = header[i];
		}

		file_tab[file_count].name[99] = '\0';
		file_tab[file_count].size = size;
		file_tab[file_count].type = header[156];
		file_tab[file_count].location = addr + 512;

		print("[INFO] File located: ");
		print(file_tab[file_count].name);
		print("' , Filesize: ");
		print_dec(size);
		print(" bytes\n");

		file_count++;

		addr += 512 + ((size + 511) & ~511);
	}

	print("[INFO] VFS initialized with ");
	print_dec(file_count);
	print(" files.\n");
}


// Locate a file, given a name, return index in the file_tab
uint32_t f_open(const char* fname)
{
	for (uint32_t i = 0; i < file_count; i++)
	{
		if (strcmp(file_tab[i].name, fname) == 0)
		{
			// Return the index of the given file
			return i;
		}
	}

	// If not found, return invalid index
	return -1;
}


// Read file's data into a buffer
uint32_t f_read(uint32_t node_idx, char* buffer, uint32_t size)
{
	if (node_idx >= file_count || buffer == 0)
	{
		return 0;
	}

	vfs_node* node = &file_tab[node_idx];
	uint32_t bytes_to_read = (size > node->size) ? node->size : size;

	char* content = (char*)node->location;

	for (uint32_t i = 0; i < bytes_to_read; i++)
	{
		buffer[i] = content[i];
	}

	return bytes_to_read;
}


// List files found in the initrd
void ls()
{
	if (file_count == 0)
	{
		print("[INFO] No files found.\n");
		return;
	}

	for (uint32_t i = 0; i < file_count; i++)
	{
		print(file_tab[i].name);

		// If it is a directory
		if (file_tab[i].type == '5')
		{
			print("/");
		}

		print("\n");
	}
}
