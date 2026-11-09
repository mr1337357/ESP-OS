#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define OS_MAX_FILES 20

#define OS_FILE_ISDIR               (1<<0)
#define OS_FILE_BLOCK_WORKAROUND    (1<<1)
#define OS_FILE_FSYNC_WORKAROUND    (1<<2)
#define OS_FILE_ECHO                (1<<3)

typedef struct 
{
    int opencount;
    int fd;
    DIR *fdir;
    uint32_t flags;
} os_filedes;

os_filedes files[OS_MAX_FILES];

static char combined_path[1024];

void filedes_init()
{
    int fd;
    for(fd = 0; fd < OS_MAX_FILES; fd++)
    {
        files[fd].opencount = 0;
        files[fd].fd = -1;
        files[fd].fdir = 0;        
        files[fd].flags = 0;
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
    files[fd].fd = fileno(adopted);
    files[fd].flags = OS_FILE_BLOCK_WORKAROUND | OS_FILE_FSYNC_WORKAROUND | OS_FILE_ECHO;
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
    if(mode & 0x200)
    {
        mode &= ~0x200;
        mode |= O_CREAT;
    }
    files[fd].fd = open(filename, mode, 0777);
    files[fd].flags = 0;
    if(files[fd].fd < 0)
    {
        files[fd].fdir = opendir(filename);
        if(!files[fd].fdir)
        {
            return -1;
        }
        files[fd].flags = OS_FILE_ISDIR;
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
        if(!(files[fd].flags & OS_FILE_ISDIR))
        {
            close(files[fd].fd);
            files[fd].fd = -1;
        }
        else
        {
            closedir(files[fd].fdir);
            files[fd].fdir = 0;
        }
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
    int blockstatus;
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    if(files[fd].flags & OS_FILE_ISDIR)
    {
        return -1;
    }
    if(files[fd].flags & OS_FILE_BLOCK_WORKAROUND)
    {
        blockstatus = os_file_block_until_read(files[fd].fd);
        if(blockstatus < 1)
        {
            return blockstatus;
        }
    }
    int rv = read(files[fd].fd, buffer, len);
    if(files[fd].flags & OS_FILE_ECHO)
    {
        if(rv > 0)
        {
            write(files[fd].fd, buffer, rv);
            fsync(files[fd].fd);
        }
    }
    return rv;
}

int os_file_write(int fd, void *buffer, int len)
{
    int rv;
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    if(files[fd].flags & OS_FILE_ISDIR)
    {
        return -1;
    }
    rv = write(files[fd].fd, buffer, len);
    if(files[fd].flags & OS_FILE_FSYNC_WORKAROUND)
    {
        fsync(files[fd].fd);
    }
    return rv;
}

int os_file_readdir(int fd, struct dirent *dent)
{
    struct dirent *d2;
    if(fd < 0 || fd >= OS_MAX_FILES)
    {
        return -1;
    }
    if(files[fd].opencount < 1)
    {
        return -1;
    }
    if(!(files[fd].flags & OS_FILE_ISDIR))
    {
        return -1;
    }
    d2 = readdir(files[fd].fdir);
    if(!d2)
    {
        return -1;
    }
    memcpy(dent,d2,sizeof(struct dirent));
    return 0;
}

char *os_file_rel2abs(const char *base, char *filename)
{
    char temp_path[1024];
    temp_path[0] = 0;
    temp_path[1023] = 0;
    if(!filename)
    {
        return 0;
    }
    if(filename[0] != '/')
    {
        strcpy(temp_path,base);
        strcat(temp_path,"/");
    }
    strcat(temp_path,filename);
    return realpath(temp_path,combined_path);
}
