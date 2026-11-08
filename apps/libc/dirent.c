#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "lib.h"

struct dir_internal
{
    int fd;
};

DIR *opendir(const char *name)
{
    struct dir_internal *dirp;
    int fd;
    fd = open(name,O_RDONLY);
    if(fd < 0)
    {
        return 0;
    }
    dirp = malloc(sizeof(struct dir_internal));
    if(!dirp)
    {
        close(fd);
        return 0;
    }
    dirp->fd = fd;
    return (DIR *)dirp;
}

struct dirent *readdir(DIR *dirp)
{
    static struct dirent dmem;
    struct dir_internal *dp = (struct dir_internal *)dirp;
    struct { int fd; struct dirent *de; } readdircall = { .fd = dp->fd, .de = &dmem};
    if(do_syscall(SYSCALL_READDIR,&readdircall)==0)
    {
        return &dmem;
    }
    return 0;
}

int closedir(DIR *dirp)
{
    struct dir_internal *dp = (struct dir_internal *)dirp;
    close(dp->fd);
    free(dp);
}