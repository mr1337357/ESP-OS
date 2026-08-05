extern int (*do_syscall)(int callnum, void *arg);

#define FILEMODE_R 0
#define FILEMODE_W 1

void print(char *message);

int write(int fd, char *buff, int len);

int read(int fd, char *buff, int len);

int open(char *name, int mode);

int close(int fd);

void delay(int ms);

void *malloc(int len);

void run(char *fname);

void event_handler(int event, void *arg);
