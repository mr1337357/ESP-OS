#include "lib.h"

void sh_launcher(void *arg)
{
   char *args[] = { "sh", 0 };
   execv("/sdcard/espos/bin/sh", args);
   execv("/internal/espos/bin/sh", args);
   print("sh not found\n");
}

int main(int argc, char **argv)
{
	thread_create(sh_launcher, 0);
	return 0;
}
