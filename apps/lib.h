#include <stddef.h>

extern int (*do_syscall)(int callnum, void *arg);

#define FILEMODE_R 0
#define FILEMODE_W 1

void print(char *message);

int write(int fd, const void *buff, size_t len);

int read(int fd, void *buff, size_t len);

int open(char *name, int mode);

int close(int fd);

size_t strlen(const char *msg);

int strcmp(char *a, char *b);

int thread_create(void *entry, void *threadargs);

int execve(const char *path, char *const argv[], char *const envp[]);

int execv(const char *path, char *const argv[]);

int waitpid(int pid, int *status, int options);

int chdir(char *path);

int getcwd(char *path, int len);

void event_handler(int event, void *arg);
