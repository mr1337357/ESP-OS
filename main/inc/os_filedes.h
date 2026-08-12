#ifndef __OS_FILEDES_H__
#define __OS_FILEDES_H__

#include <stdio.h>

void filedes_init();

int os_file_open(char *filename, int mode);

int os_file_claim(int fd);

int os_file_close(int fd);

int os_file_read(int fd, void *buffer, int len);

int os_file_write(int fd, void *buffer, int len);

FILE *os_get_filep(int fd);

#endif