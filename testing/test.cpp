// Implementation of the kernel's testing framework

#include "test.h"
#include "../kernel.h"

#define MAX_TESTS 16

static testcase_t test_reg[MAX_TESTS];
static int test_count = 0;

// Forward declaration of test functions
void test_mem();
void test_vfs();

// Register a new test case
void test_add(const char* description, void (*fn)())
{
    if (test_count < MAX_TESTS)
    {
        test_reg[test_count].desc = description;
        test_reg[test_count].func = fn;
        test_count++;
    }
}

// Initialize/register test cases
void test_init()
{
    // Tests that are available

    // Kernel memory tests
    test_add("Kernel memory allocation", &test_mem);
    // VFS testing
    //test_add("Virtual File System (VFS) functions", &test_vfs);
}


// List available tests
void test_list()
{
    print("Available tests:\n");

    for (int i = 0; i < test_count; i++)
    {
        print(" ");
        print_dec(i + 1);
        print(": ");
        print(test_reg[i].desc);
        print("\n");
    }
}

// Execute a specific test by index
void test_exec(int test_num)
{
    int test_idx = test_num - 1;

    if (test_idx < 0 || test_idx >= test_count)
    {
        print("[ERR] No such test. For a list of available test cases, use the 'test list' command.\n");
        return;
    }

    print("[INFO] Test-case ");
    print_dec(test_num);
    print(" in progress.\n");
    print("Description: ");
    print(test_reg[test_idx].desc);
    print("\n");

    // Actually run the test function
    test_reg[test_idx].func();
    print("[INFO] Testing complete.\n");
}