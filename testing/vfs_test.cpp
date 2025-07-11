// These tests are designed to verify the integrity of the kernel's initrd

#include "test.h"
#include "../error.h"
#include "../kernel.h"
#include "../vfs.h"

void test_vfs()
{
    log(LOG_INFO, "Verifying the integrity of the VFS\n");

    extern uint32_t file_get_count();
    extern void f_stat_by_idx(uint32_t idx, vfs_node* node);

    uint32_t count = file_get_count();
    log(LOG_INFO, "VFS file count: ");
    print_dec(count);
    log(LOG_INFO, "\n");


    for (uint32_t i = 0; i < count; ++i)
    {
        vfs_node node;
        f_stat_by_idx(i, &node);

        bool null_term = false;

        for (int j = 0; j < 100; ++j)
        {
            if (node.name[j] == '\0')
            {
                null_term = true;
                break;
            }
        }

        if (!null_term)
        {
            log(LOG_WARN, "File name is not null-terminated: ");

            for (int j = 0; j < 100; ++j)
            {
                print_char(node.name[j]);
                print("\n");
            }
        }

        // Make sure characters in file names are printable
        for (int j = 0; node.name[j] && j < 100; ++j)
        {
            if (node.name[j] < 32 || node.name[j] > 126)
            {
                log(LOG_WARN, "File name contains non-printable characters: ");
                for (int k = 0; k < 100; ++k)
                {
                    print_char(node.name[k]);
                }
                print("\n");
                break;
            }
        }

        // Print file information
        log(LOG_INFO, "File: ");
        print(node.name);
        log(LOG_INFO, "| Size: ");
        print_dec(node.size);
        log(LOG_INFO, "| Type: ");
        print_char(node.type);
        print("\n");
    }

    log(LOG_INFO, "Testing completed.\n");
}