#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int putchar(int c)
{
    write(1,&c,1);
    return c;
}

int puts(const char *s)
{
    write(1,s,strlen(s));
    write(1,"\n",1);
    return strlen(s);
}

int	printf (const char *__restrict format, ...)
{
    write(1,format,strlen(format));
    return strlen(format);
}