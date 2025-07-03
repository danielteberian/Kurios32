// This header file establishes the testing framework for the kernel

#ifndef TEST_H
#define TEST_H

// Define a single test-case
typedef struct testcase_t
{
    // A description of the test case
    const char* desc;
    void (*func)();
};

// Initialize the testing registry
void test_init();
// List available test cases
void test_list();
// Execute a specific test case by index
void test_exec(int test_id);

#endif