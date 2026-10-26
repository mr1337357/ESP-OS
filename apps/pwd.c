#include "lib.h"

int main(int argc, char **argv)
{
    char cwd[1024];
    getcwd(cwd, 1024);
    print(cwd);
    print("\n");
    return 0;
}