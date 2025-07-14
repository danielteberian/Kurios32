// This header declares error handling/logging functions for the kernel

#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

// These are the types of logging available to the kernel
typedef enum
{
    // First level of logging, used to inform the user
    LOG_INFO,
    // Second level of logging, used to warn the user of a potential issue
    LOG_WARN,
    // Third level of logging, used to alert the user that there is an error
    LOG_ERROR,
    // The fourth and highest level of logging, when everything falls apart
    LOG_FATAL
} log_lvl_t;

void log(log_lvl_t lvl, const char* msg);
void punchout(const char* msg);

#endif