// Header file for the kernel's VFS

#ifndef VFS_H
#define VFS_H

#include "multiboot.h"
#include <stdint.h>

#define MAX_FILES 16

// Structure for a file/directory
struct vfs_node
{
	// File/directory name, maximum length is 100 characters
	char name[100];
	// The size of the file/directory
	uint32_t size;
	// Where the file/directory is located in the memory
	uint32_t location;
	// Whether the node is a file (0) or a directory (5)
	uint32_t type;
};

// The API
void vfs_init(mb_info_t* mbt);

void f_stat(uint32_t node_idx, vfs_node* node_out);
uint32_t f_open(const char* fname);
uint32_t f_read(uint32_t node_idx, char* buffer, uint32_t size);
// FINALLY
void ls();

#endif
