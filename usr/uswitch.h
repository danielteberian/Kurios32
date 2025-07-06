// Header file for switching to usermode

#ifndef USWITCH_H
#define USWITCH_H

#include <stdint.h>

void enter_umode(uint32_t u_eip, uint32_t u_esp);

#endif