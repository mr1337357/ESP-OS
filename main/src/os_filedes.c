#include <stdio.h>
#include <sys/select.h>
#include <errno.h>

#define OS_MAX_FILES 20

typedef struct 
{
    int opencount;
    FILE *filep;
} os_filedes;

os_filedes files[OS_MAX_FILES];

void filedes_init()
{
    int fd;
    for(fd = 0; fd < OS_MAX_FILES; fd++)
    {
        files[fd].filep = 0;
        files[fd].opencount = 0;
    }
}

int os_file_adopt(FILE *adopted)
{
    int fd;
    for(fd=0;fd<OS_MAX_FILES;fd++)
    {
        if(files[fd].opencount == 0)
        {
            break;
        }
    }
    if(fd == OS_MAX_FILES)
    {
        return -1;
    }
    files[fd].opencount = 1;
    files[fd].filep = adopted;
    return fd;
}

int os_file_open(char *filename, int mode)
{
    int fd;
    for(fd=0;fd<OS_MAX_FILES;fd++)
    {
        if(files[fd].opencount == 0)
        {
            break;
        }
    }
    if(fd == OS_MAX_FILES)
    {
        return -1;
    }
    files[fd].filep = fopen(filename,mode == 1?"w":"r");
    if(files[fd].filep == 0)
    {
        return -1;
    }
    files[fd].opencount = 1;
    return fd;
}

int os_file_claim(int fd)
{
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    files[fd].opencount++;
    return 0;
}

int os_file_close(int fd)
{
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    files[fd].opencount--;
    if(files[fd].opencount == 0)
    {
        fclose(files[fd].filep);
        files[fd].filep = 0;
    }
    return 0;
}

int os_file_block_until_read(int fd)
{
    int s = 0;
    fd_set rfds;
    struct timeval tv = {
        .tv_sec = 1,
        .tv_usec = 0,
    };

    while(s == 0)
    {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        s = select(fd + 1, &rfds, NULL, NULL, &tv);
        if(s < 0 && errno == EINTR)
        {
            s = 0;
        }
    }
    return s;
}

int os_file_read(int fd, void *buffer, int len)
{
    int actual_fd;
    int blockstatus;
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    actual_fd = fileno(files[fd].filep);
    blockstatus = os_file_block_until_read(actual_fd);
    if(blockstatus < 1)
    {
        return blockstatus;
    }
    return fread(buffer, 1, len, files[fd].filep);
}

int os_file_write(int fd, void *buffer, int len)
{
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    return fwrite(buffer, 1, len, files[fd].filep);
}

FILE *os_get_filep(int fd)
{
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return 0;
    }
    return files[fd].filep;
}
