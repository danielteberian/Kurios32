// Error handling and logging for the kernel

#include "error.h"
#include "kernel.h"


// Strings for each log level
static const char* log_lvl_str[] =
{
    "INFO",
    "ALERT",
    "ERR",
    "!!!"
};

// Colors for each of the log levels
void (*log_lvl_colors[])(const char*) =
{
    print_cyan,
    print_yellow,
    print_red,
    print_red
};

void log(log_lvl_t lvl, const char* msg)
{
    char buffer[256];
    const char* lvl_str = log_lvl_str[lvl];
    void (*color_print)(const char*) = log_lvl_colors[lvl];
    
    // Formatting for the logging messages
    int len = 0;
    buffer[len++] = '[';

    for (int i = 0; lvl_str[i] != '\0'; i++)
    {
        buffer[len++] = lvl_str[i];
    }

    buffer[len++] = ']';
    buffer[len++] = ' ';

    for (int i = 0; msg[i] != '\0'; ++i)
    {
        buffer[len++] = msg[i];
    }

    buffer[len++] = '\0';

    color_print(buffer);
    print_char('\n');
}

void punchout(const char* msg)
{
    log (LOG_FATAL, msg);

    for (;;)
    {
        asm("cli; hlt");
    }
}