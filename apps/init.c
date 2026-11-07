#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libc.h"

void sh_launcher(void *arg)
{
   char *args[] = { "sh", 0 };
   execv("/sdcard/espos/bin/sh", args);
   execv("/internal/espos/bin/sh", args);
   printf("sh not found\n");
}

int main(int argc, char **argv)
{
   char *pidnum = "pid  \n";
   int pid;
   printf("init loaded... starting shell\n");
	while(1)
   {
      pid = thread_create(sh_launcher, 0);
      pidnum[4] = '0'+pid;
      printf(pidnum);
      waitpid(pid,0,0);
      printf("waitpid done\n");
   }
	return 0;
}
