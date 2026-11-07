#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char cwd[1024];
    getcwd(cwd, 1024);
    printf(cwd);
    printf("\n");
    return 0;
}