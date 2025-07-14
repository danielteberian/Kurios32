// A basic VFS implementation for the kernel

#include <stddef.h>
#include <stdint.h>

#include "include/kernel.h"
#include "include/mem.h"
#include "include/vfs.h"


// Forward declaration
int strcmp(const char* str1, const char* str2);

// This array stores information about the files in the initrd
static vfs_node file_tab[MAX_FILES];
static uint32_t file_count = 0;
// Timestamp stuff
static uint32_t vfs_time = 0;
static uint32_t next_f_loc = 0x100000;


// Create a file
uint32_t vfs_new(const char* name)
{
	if (file_count >= MAX_FILES)
	{
		// No space
		return (uint32_t) - 1;
	}

	for (uint32_t i = 0; i < file_count; ++i)
	{
		if (strcmp(file_tab[i].name, name) == 0)
		{
			return i;
		}
	}

	vfs_node* node = &file_tab[file_count];

	for (int i = 0; i < 100; ++i)
	{
		node -> name[i] = 0;
	}

	for (int i = 0; name[i] && i < 99; ++i)
	{
		node -> name[i] = name[i];
	}

	node -> size = 0;
	node -> location = next_f_loc;
	node -> type = 0;
	node -> created = vfs_time++;
	node -> modified = node -> created;
	// 1KB by default
	node -> data = (char*)kmalloc(1024);

	++file_count;
	next_f_loc += 1024;
	return file_count - 1;
}


uint32_t fwrite(uint32_t node_idx, const char* buffer, uint32_t len)
{
	if (node_idx >= file_count)
	{
		return 0;
	}

	vfs_node* node = &file_tab[node_idx];

	// This limit is temporary.
	if (len > 1024)
	{
		len = 1024;
	}

	for (uint32_t i = 0; i < len; ++i)
	{
		node -> data[i] = buffer[i];
	}

	node -> size = len;
	node -> modified = vfs_time++;

	return len;
}


void vfs_update_timestamp(uint32_t node_idx)
{
	if (node_idx >= file_count)
	{
		return;
	}

	file_tab[node_idx].modified = vfs_time++;
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
		print("\n[INFO] No files found.\n");
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


// Count files
uint32_t file_get_count()
{
	return file_count;
}

// Get information about VFS node via direct index in the file table
void f_stat_by_idx(uint32_t idx, vfs_node* node_out)
{
	if (idx < file_count && node_out != 0)
	{
		*node_out = file_tab[idx];
	}
}
