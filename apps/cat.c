#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int readlen;
    int fd;
    char buffer[128];
    if(argc >= 2)
    {
        fd = open(argv[1],O_RDONLY);
    }
    else
    {
        fd = 0;
    }
    if(fd < 0)
    {
        printf("could not open file\n");
        return 1;
    }
    
    while((readlen = read(fd,&buffer,128))>0)
    {
        write(1,&buffer,readlen);
    }
    close(fd);
    return 0;
}