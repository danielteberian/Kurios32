// Multiboot header (not that kind)

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_INFO_MEMORY       0x00000001
#define MULTIBOOT_INFO_BOOTDEV      0x00000002
#define MULTIBOOT_INFO_CMDLINE      0x00000004
#define MULTIBOOT_INFO_MODS         0x00000008


typedef struct mb_mod
{
    uint32_t mod_start;
    uint32_t mod_end;
    char* str;
    uint32_t reserved;
}
mb_mod_t;


typedef struct mb_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_dev;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
}
mb_info_t;

#endif