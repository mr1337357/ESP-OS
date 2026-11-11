#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

FILE _stdin = { ._file = 0 };
FILE _stdout = { ._file = 1 };
FILE _stderr = { ._file = 2 };

void stdio_init()
{
stdin = &_stdin;
stdout = &_stdout;
stderr = &_stderr;

}

FILE *fopen(const char *restrict path, const char *restrict mode)
{
    int fd;
    int fdmode = O_CREAT;
    int i;
    FILE *outfile;
    for(i=0;mode[i];i++)
    {
        switch(mode[i])
        {
            case 'r':
                fdmode |= O_RDONLY;
                break;
            case 'w':
                fdmode |= O_WRONLY;
                break;
            case 'a':
                fdmode |= O_APPEND;
                break;
        }
    }
    fd = open(path,fdmode,0777);
    if(fd < 0)
    {
        return 0;
    }
    outfile = malloc(sizeof(FILE));
    if(!outfile)
    {
        return 0;
    }
    outfile->_file = fd;
    return outfile;
}

size_t fread(void *restrict buffer, size_t size, size_t n, FILE *restrict stream)
{
    return read(stream->_file,buffer,size * n);
}

size_t fwrite(const void *restrict buffer, size_t size, size_t n, FILE *restrict stream)
{
    return write(stream->_file,buffer,size * n);
}

int fclose(FILE *stream)
{
    close(stream->_file);
    free(stream);
    return 0;
}

int putchar(int c)
{
    fwrite(&c, 1, 1, stdout);
    return c;
}

int fputs(const char *s, FILE *stream)
{
    fwrite(s, 1, strlen(s), stream);
    return strlen(s);
}

int puts(const char *s)
{
    fwrite(s, 1, strlen(s), stdout);
    fwrite("\n", 1, 1, stdout);
    return strlen(s);
}

static void append_int(int num, char **buffer)
{
    int i;
    char temp_buffer[11];
    int started = 0;
    char *localbuffer = *buffer;
    if(num < 0)
    {
        *localbuffer++ = '-';
        num = -num;
    }
    for(i=9;i>=0;i--)
    {
        temp_buffer[i] = (num%10)+'0';
        num /= 10;
    }
    temp_buffer[10] = 0;
    for(i=0;i<10;i++)
    {
        if(started || temp_buffer[i]>'0')
        {
            started = 1;
            *localbuffer++ = temp_buffer[i];
        }
    }
    if(!started)
    {
        *localbuffer++ = '0';
    }
    *buffer = localbuffer;
}

static void append_hex(uint32_t num, char **buffer)
{
    const char *hex = "0123456789ABCDEF";
    int i;
    int started = 0;
    char *localbuffer = *buffer;
    for(i = 0; i < 8; i++)
    {
        if(started || (num>>28))
        {
            started = 1;
            *localbuffer++ = hex[num>>28];
        }
        num<<=4;
    }
    if(!started)
    {
        *localbuffer++ = '0';
    }
    *buffer = localbuffer;
}

static void append_str(char *str, char **buffer)
{
    *buffer = stpcpy(*buffer,str);
}

int vsprintf(char *restrict str, const char *restrict format, va_list ap)
{
    int inlen;
    int i;
    char *outp;
    outp = str;
    inlen = strlen(format);
    for(i=0;i<inlen;i++)
    {
        if(format[i] != '%')
        {
            *outp++ = format[i];
        }
        else
        {
            i++;
            switch(format[i])
            {
                case 'd':
                    append_int(va_arg(ap,int32_t),&outp);
                    break;
                case 'x':
                    append_hex(va_arg(ap,uint32_t),&outp);
                    break;
                case 's':
                    append_str(va_arg(ap,char *),&outp);
                    break;
            }
        }
    }
    *outp = 0;
}

int	printf (const char *__restrict format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args,format);
    vsprintf(buffer,format,args);
    va_end(args);
    fputs(buffer,stdout);
    //write(1,format,strlen(format));
    return strlen(format);
}