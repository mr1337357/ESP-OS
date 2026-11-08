#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    struct dirent *d;
    DIR *here = opendir(".");
    if(!here)
    {
        printf("error: could not open dir\n");
        return 1;
    }
    for(d = readdir(here); d; d = readdir(here))
    {
        printf(d->d_name);
        printf("\n");
    }
    return 0;
}