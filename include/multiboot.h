// Multiboot header (not that kind)

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_INFO_MEMORY       0x00000001
#define MULTIBOOT_INFO_BOOTDEV      0x00000002
#define MULTIBOOT_INFO_CMDLINE      0x00000004
#define MULTIBOOT_INFO_MODS         0x00000008
#define MULTIBOOT_INFO_AOUT_SYMS    0x00000010
#define MULTIBOOT_INFO_ELF_SHDR     0x00000020
#define MULTIBOOT_INFO_MEM_MAP      0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO   0x00000080
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200
#define MULTIBOOT_INFO_APM_TABLE    0x00000400
#define MULTIBOOT_INFO_VBE          0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER  0x00001000


typedef struct mb_mod
{
    uint32_t mod_start;
    uint32_t mod_end;
    char* str;
    uint32_t reserved;
}
__attribute__((packed)) mb_mod_t;


typedef struct mb_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_dev;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;

    uint32_t elf_sec_num;
    uint32_t elf_sec_size;
    uint32_t elf_sec_addr;
    uint32_t elf_sec_shndx;

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;

    uint8_t color_info[6];
}
__attribute__((packed)) mb_info_t;

#endif