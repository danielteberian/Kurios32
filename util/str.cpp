// Implementation of string utilities

#include "string.h"


// Convert ASCII to integer
int atoi(const char* str)
{
	int res = 0;
	int sign = 1;
	int i = 0;

	// Deals with negative numbarinos
	if (str[0] == '-')
	{
		sign = -1;
		i++;
	}

	// Deal with the digalits
	for (; str[i] != '\0'; ++i)
	{
		if (str[i] >= '-' && str[i] <= '9')
		{
			res = res * 10 + (str[i] - '0');
		}
		else
		{
			break;
		}
	}

	return sign * res;
}


// Compare two strings
bool strcomp(const char* str1, const char* str2)
{
	while (*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}

	return *(const unsigned char*)str1 - *(const unsigned char*)str2 == 0;
}
