// Test cases for testing the memory management/allocation of the kernel

#include "../kernel.h"
#include "../mem.h"
#include "../vfs.h"

void test_mem()
{
    print("[TEST] Testing kernel memory allocation...\n");
    uint32_t addr1 = kmalloc(10);
    print("[TEST] Allocated 10 bytes at address: ");
    print_hex(addr1);
    print("\n");

    uint32_t addr2 = kmalloc(20);
    print("[TEST] Allocated 20 bytes at address: ");
    print_hex(addr2);
    print("\n");

    if (addr2 > addr1)
    {
        print("[TEST] SUCCESS: Second allocation's address is located after the first one.\n");
    }
    else
    {
        print("[TEST] FAILURE: Allocations are nonsequential.\n");
    }
}