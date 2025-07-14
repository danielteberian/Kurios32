// The kernel's conversion utility

#include "convert.h"
#include "../include/kernel.h"
#include "../include/sh.h"


// The types of units that can be converted. This makes sure that only compatible units can be converted
enum unit_type
{
    // bytes
    // Bases
    // speed/velocity
    // temperature
    // mass
    // liquid measurements
    u_length,
    u_weight,
};



// Definition of a unit (me, basically)
struct unit
{
    // The full name of the unit
    const char* name;
    // Abbreviation
    const char* abbr;
    // Unit type
    unit_type type;
    // Factor used to convert the unit to its base (km to m, for example)
    double factor;
};


// Registry of units
// The first unit of a type is the base unit.
static const unit unit_reg[] =
{
    // Length
    {"foot", "ft", u_length, 1.0},
    {"inch", "in", u_length, 1 / 12.0},
    {"yard", "yd", u_length, 3.0},
    {"mile", "mi", u_length, 5280.0},
    {"meter", "m", u_length, 3.28084},
    {"kilometer", "km", u_length, 3280.84},
    {"centimeter", "cm", u_length, 0.0328084},
    {"millimeter", "mm", u_length, 0.00328084},

    // Weight
    {"pound", "lb", u_weight, 1.0},
    {"ounce", "oz", u_weight, 1 / 16.0},
    {"ton", "t", u_weight, 2000.0},
    {"tonne", "tn", u_weight, 2204.62},
    {"stone", "st", u_weight, 14.0},
    {"gram", "g", u_weight, 0.00220462},
    {"kilogram", "kg", u_weight, 2.20462},
    {"milligram", "mg", u_weight, 0.00000220462},
};

static const int unit_count = sizeof(unit_reg) / sizeof(unit);

// Use name/abbreviation to locate a specific unit from the registry
const unit* find_unit(const char* name)
{
    for (int i = 0; i < unit_count; ++i)
    {
        if (strcmp(name, unit_reg[i].name) == 0 || strcmp(name, unit_reg[i].abbr) == 0)
        {
            return &unit_reg[i];
        }
    }

    return nullptr;
}


// Convert a string to float
double atof(const char* str)
{
    double res = 0.0;
    double fraction = 0.0;
    double divisor = 1.0;
    int sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9')
    {
        res = res * 10.0 + (*str - '0');
        str++;
    }

    if (*str == '.')
    {
        str++;

        while (*str >= '0' && *str <= '9')
        {
            fraction = fraction * 10.0 + (*str - '0');
            divisor *= 10.0;
            str++;
        }
    }

    return sign * (res + fraction / divisor);
}



// List available units
void units_list()
{
    print ("\nAvailable units: \n");

    for (int i = 0; i < unit_count; ++i)
    {
        print_white("  - ");
        print_white(unit_reg[i].name);
        print_white(" ( ");
        print_white(unit_reg[i].abbr);
        print_white(" )\n");
    }
}


void convert_cmd_handler(const char* args)
{
    char val_str[32] = {0};
    char base_str[32] = {0};
    char target_str[32] = {0};

    int part = 0;
    int i = 0;

    while (*args)
    {
        if (*args == ' ')
        {
            if (i > 0)
            {
                part++;
                i = 0;
            }

            args++;
            continue;
        }

        if (part == 0 && i < 32)
        {
            val_str[i++] = *args;
        }

        else if (part == 1 && i < 31)
        {
            base_str[i++] = *args;
        }

        else if (part == 2 && i < 31)
        {
            target_str[i++] = *args;
        }

        args++;
    }

    if (part == 0 && strcmp(val_str, "list") == 0)
    {
        units_list();
        return;
    }

    if (part != 2)
    {
        print_yellow("\nConversion Utility\n");
        print("Usage: convert <value> <base unit> <target unit>\n");
        print("For a list of units, use 'convert list'\n");
        return;
    }

    const unit* base_unit = find_unit(base_str);
    const unit* target_unit = find_unit(target_str);

    if (!base_unit || !target_unit)
    {
        print_red("\n[ERR]: Invalid unit.\n");
        return;
    }

    if (base_unit -> type != target_unit -> type)
    {
        print_red("\n[ERR]: Units are incompatible.\n");
        return;
    }

    double val = atof(val_str);
    double val_base = val * base_unit -> factor;
    double result = val_base / target_unit -> factor;

    print_fl(val);
    print(" ");
    print(base_unit -> name);
    print(" = ");
    print_fl(result);
    print(" ");
}

// Makes convert available in the shell
void convert_init()
{
    reg_sh_cmd("convert", convert_cmd_handler);
}
