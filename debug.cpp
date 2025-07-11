#include "debug.h"
#include "kernel.h"

#include <stddef.h>

static const char* d_lvl_str[] = 
{
    "[DEB]",
    "[ALERT]",
    "[ERR]",
};

void dlog(int lvl, const char* msg)
{
    if (lvl < 0 || lvl > 2)
    {
        lvl = 0;
    }

    print(d_lvl_str[lvl]);
    print(" ");
    print(msg);
    print("\n");
}

void memdump(uint32_t addr, uint32_t count)
{
    print("[MEM] ");
    print_hex(addr);
    print(" - ");
    print_dec(count);
    print(" bytes: \n");

    for (uint32_t i = 0; i < count; i++)
    {
        if (i % 16 == 0)
        {
            print("  ");
            print_hex(addr + i);
            print(": ");
        }

        uint8_t value = *((uint8_t*)(addr + i));
        const char* hex = "0123456789ABCDEF";
        char out[3];
        out[0] = hex[(value >> 4) & 0x0F];
        out[1] = hex[value & 0xF];
        out[2] = 0;
        print(out);
        print(" ");

        if ((i + 1) % 16 == 0)
        {
            print("\n");
        }
    }

    if (count % 16 != 0)
    {
        print("\n");
    }
}

typedef void (*sh_cmd_f)(const char* args);
extern "C" void reg_sh_cmd(const char* name, sh_cmd_f handler);

static void dcmd(const char* args)
{
    if (!args || args[0] == 0)
    {
        print("Usage: debug log <msg> | debug mem <addr> [count]\n");
        return;
    }

    while (*args == ' ')
    {
        ++args;
    }

    if (args[0] == 'l' && args[1] == 'o' && args[2] == 'g')
    {
        const char* msg = args + 3;

        while (*msg == ' ')
        {
            ++msg;
        }

        dlog(0, msg);
    }
    else if (args[0] == 'm' && args[1] == 'e' && args[2] == 'm')
    {
        const char* p = args + 3;

        while (*p == ' ')
        {
            ++p;
        }

        uint32_t addr = 0;
        uint32_t count = 0;

        if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        {
            p += 2;
            while ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))
            {
                addr <<= 4;
                if (*p >= '0' && *p <= '9')
                {
                    addr |= (*p - '0');
                }
                else if (*p >= 'a' && *p <= 'f')
                {
                    addr |= (*p - 'a' + 10);
                }
                else if (*p >= 'A' && *p <= 'F')
                {
                    addr |= (*p - 'A' + 10);
                }
                ++p;
            }
        }
        else
        {
            while (*p >= '0' && *p <= '9')
            {
                addr = addr * 10 + (*p++ - '0');
            }
        }

        while (*p == ' ')
        {
            ++p;
        }

        if (*p != '\0')
        {
            if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
            {
                p += 2;
                while ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))
                {
                    count <<= 4;
                    if (*p >= '0' && *p <= '9')
                    {
                        count |= (*p - '0');
                    }
                    else if (*p >= 'a' && *p <= 'f')
                    {
                        count |= (*p - 'a' + 10);
                    }
                    else if (*p >= 'A' && *p <= 'F')
                    {
                        count |= (*p - 'A' + 10);
                    }
                    ++p;
                }
            }
            else
            {
                while (*p >= '0' && *p <= '9')
                {
                    count = count * 10 + (*p++ - '0');
                }
            }
        }

        if (count == 0)
        {
            count = 16;
        }

        memdump(addr, count);
    }
    else
    {
        print("Usage: debug log <msg> | debug mem <addr> [count]\n");
    }
}


void dregsh()
{
    reg_sh_cmd("debug", dcmd);
}