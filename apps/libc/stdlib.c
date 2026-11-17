#include <stdlib.h>

int atoi(const char *nptr)
{
    int neg = 1;
    int output = 0;
    if(*nptr == '-')
    {
        nptr++;
        neg = -1;
    }
    while(*nptr >= '0' && *nptr <= '9')
    {
        output *= 10;
        output += *nptr-'0';
        nptr++;
    }
    return output * neg;
}