#include <stdint.h>
#include "include/paging.h"


//  Defines an ELF header
typedef struct
{
    uint8_t ident;
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
} elf_header_t;


// Program header
typedef struct
{
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} elf_program_header_t;

#define PT_LOAD 1

// Load ELF executable from buffer into a specified page directory
uint32_t load_elf(page_dir_t* dir, uint8_t* elf_dat);
