
sh:     file format elf32-xtensa-le


Disassembly of section .text:

00400074 <_start-0x1c>:
  400074:	004012bc 	
  400078:	004012c0 	
  40007c:	00400258 	
  400080:	004002a8 	
  400084:	004002b4 	
  400088:	00400190 	
  40008c:	004000ec 	

00400090 <_start>:
int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(struct exec_args *startargs)//void *syscall_handle, int argc, char **argv)
{
  400090:	006136        	entry	a1, 48
  400093:	017d      	mov.n	a7, a1
  400095:	0729      	s32i.n	a2, a7, 0
   __environ = 0;
  400097:	fff781        	l32r	a8, 400074 <_start-0x1c> (4012bc <__environ>)
  40009a:	090c      	movi.n	a9, 0
  40009c:	0899      	s32i.n	a9, a8, 0
   do_syscall = startargs->syscall;
  40009e:	0788      	l32i.n	a8, a7, 0
  4000a0:	0898      	l32i.n	a9, a8, 0
  4000a2:	fff581        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  4000a5:	0899      	s32i.n	a9, a8, 0
   //struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   //do_syscall = syscall_handle;
   //do_syscall(0, &eventcall);
   main(startargs->argc,startargs->argv);
  4000a7:	0788      	l32i.n	a8, a7, 0
  4000a9:	1898      	l32i.n	a9, a8, 4
  4000ab:	0788      	l32i.n	a8, a7, 0
  4000ad:	2888      	l32i.n	a8, a8, 8
  4000af:	08bd      	mov.n	a11, a8
  4000b1:	09ad      	mov.n	a10, a9
  4000b3:	fff281        	l32r	a8, 40007c <_start-0x14> (400258 <main>)
  4000b6:	0008e0        	callx8	a8
}
  4000b9:	f03d      	nop.n
  4000bb:	f01d      	retw.n
  4000bd:	000000        	ill

004000c0 <write>:

int write(int fd, const void *buff, size_t len)
{
  4000c0:	008136        	entry	a1, 64
  4000c3:	017d      	mov.n	a7, a1
  4000c5:	4729      	s32i.n	a2, a7, 16
  4000c7:	5739      	s32i.n	a3, a7, 20
  4000c9:	6749      	s32i.n	a4, a7, 24
   
   fileop fop = { .fd = fd, .buffer = (void *)buff, .len = len}; //drop the const (the kernel won't write here so it should be ok)
  4000cb:	4788      	l32i.n	a8, a7, 16
  4000cd:	0789      	s32i.n	a8, a7, 0
  4000cf:	6788      	l32i.n	a8, a7, 24
  4000d1:	1789      	s32i.n	a8, a7, 4
  4000d3:	5788      	l32i.n	a8, a7, 20
  4000d5:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_WRITE,&fop);
  4000d7:	ffe881        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  4000da:	0888      	l32i.n	a8, a8, 0
  4000dc:	07bd      	mov.n	a11, a7
  4000de:	01a0a2        	movi	a10, 1
  4000e1:	0008e0        	callx8	a8
  4000e4:	0a8d      	mov.n	a8, a10
}
  4000e6:	082d      	mov.n	a2, a8
  4000e8:	f01d      	retw.n
	...

004000ec <read>:

int read(int fd, void *buff, size_t len)
{
  4000ec:	008136        	entry	a1, 64
  4000ef:	017d      	mov.n	a7, a1
  4000f1:	4729      	s32i.n	a2, a7, 16
  4000f3:	5739      	s32i.n	a3, a7, 20
  4000f5:	6749      	s32i.n	a4, a7, 24
   fileop fop = { .fd = fd, .buffer = (void *)buff, .len = len}; //drop the const (the kernel won't write here so it should be ok)
  4000f7:	4788      	l32i.n	a8, a7, 16
  4000f9:	0789      	s32i.n	a8, a7, 0
  4000fb:	6788      	l32i.n	a8, a7, 24
  4000fd:	1789      	s32i.n	a8, a7, 4
  4000ff:	5788      	l32i.n	a8, a7, 20
  400101:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_READ,&fop);
  400103:	ffdd81        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  400106:	0888      	l32i.n	a8, a8, 0
  400108:	07bd      	mov.n	a11, a7
  40010a:	00a0a2        	movi	a10, 0
  40010d:	0008e0        	callx8	a8
  400110:	0a8d      	mov.n	a8, a10
}
  400112:	082d      	mov.n	a2, a8
  400114:	f01d      	retw.n
	...

00400118 <open>:

int open(char *name, int mode)
{
  400118:	008136        	entry	a1, 64
  40011b:	017d      	mov.n	a7, a1
  40011d:	4729      	s32i.n	a2, a7, 16
  40011f:	5739      	s32i.n	a3, a7, 20
   fileop fop = { .fd = mode, .buffer = (void *)name, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
  400121:	5788      	l32i.n	a8, a7, 20
  400123:	0789      	s32i.n	a8, a7, 0
  400125:	080c      	movi.n	a8, 0
  400127:	1789      	s32i.n	a8, a7, 4
  400129:	4788      	l32i.n	a8, a7, 16
  40012b:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_OPEN,&fop);
  40012d:	ffd281        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  400130:	0888      	l32i.n	a8, a8, 0
  400132:	07bd      	mov.n	a11, a7
  400134:	2a0c      	movi.n	a10, 2
  400136:	0008e0        	callx8	a8
  400139:	0a8d      	mov.n	a8, a10
}
  40013b:	082d      	mov.n	a2, a8
  40013d:	f01d      	retw.n
	...

00400140 <close>:

int close(int fd)
{
  400140:	008136        	entry	a1, 64
  400143:	017d      	mov.n	a7, a1
  400145:	4729      	s32i.n	a2, a7, 16
   fileop fop = { .fd = fd, .buffer = 0, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
  400147:	4788      	l32i.n	a8, a7, 16
  400149:	0789      	s32i.n	a8, a7, 0
  40014b:	080c      	movi.n	a8, 0
  40014d:	1789      	s32i.n	a8, a7, 4
  40014f:	080c      	movi.n	a8, 0
  400151:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_CLOSE,&fop);
  400153:	ffc981        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  400156:	0888      	l32i.n	a8, a8, 0
  400158:	07bd      	mov.n	a11, a7
  40015a:	03a0a2        	movi	a10, 3
  40015d:	0008e0        	callx8	a8
  400160:	0a8d      	mov.n	a8, a10
}
  400162:	082d      	mov.n	a2, a8
  400164:	f01d      	retw.n
	...

00400168 <strlen>:

size_t strlen(const char *msg)
{
  400168:	008136        	entry	a1, 64
  40016b:	017d      	mov.n	a7, a1
  40016d:	4729      	s32i.n	a2, a7, 16
   int i;
   for(i=0;*msg++;i++)
  40016f:	080c      	movi.n	a8, 0
  400171:	0789      	s32i.n	a8, a7, 0
  400173:	000146        	j	40017c <strlen+0x14>
  400176:	0788      	l32i.n	a8, a7, 0
  400178:	881b      	addi.n	a8, a8, 1
  40017a:	0789      	s32i.n	a8, a7, 0
  40017c:	4788      	l32i.n	a8, a7, 16
  40017e:	981b      	addi.n	a9, a8, 1
  400180:	4799      	s32i.n	a9, a7, 16
  400182:	000882        	l8ui	a8, a8, 0
  400185:	fed856        	bnez	a8, 400176 <strlen+0xe>
   {
   }
   return i;
  400188:	0788      	l32i.n	a8, a7, 0
}
  40018a:	082d      	mov.n	a2, a8
  40018c:	f01d      	retw.n
	...

00400190 <print>:

void print(char *message)
{
  400190:	006136        	entry	a1, 48
  400193:	017d      	mov.n	a7, a1
  400195:	0729      	s32i.n	a2, a7, 0
   write(1,message,strlen(message));
  400197:	0027a2        	l32i	a10, a7, 0
  40019a:	fffce5        	call8	400168 <strlen>
  40019d:	0a8d      	mov.n	a8, a10
  40019f:	08cd      	mov.n	a12, a8
  4001a1:	07b8      	l32i.n	a11, a7, 0
  4001a3:	1a0c      	movi.n	a10, 1
  4001a5:	fff1a5        	call8	4000c0 <write>
}
  4001a8:	f03d      	nop.n
  4001aa:	f01d      	retw.n

004001ac <thread_create>:

int thread_create(void *entry, void *threadargs)
{
  4001ac:	008136        	entry	a1, 64
  4001af:	017d      	mov.n	a7, a1
  4001b1:	4729      	s32i.n	a2, a7, 16
  4001b3:	5739      	s32i.n	a3, a7, 20
   struct {void *entry; void *args;} threadcall = {.entry = entry, .args = threadargs};
  4001b5:	4788      	l32i.n	a8, a7, 16
  4001b7:	0789      	s32i.n	a8, a7, 0
  4001b9:	5788      	l32i.n	a8, a7, 20
  4001bb:	1789      	s32i.n	a8, a7, 4
   return do_syscall(SYSCALL_CLONE, &threadcall);
  4001bd:	ffae81        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  4001c0:	0888      	l32i.n	a8, a8, 0
  4001c2:	07bd      	mov.n	a11, a7
  4001c4:	4a0c      	movi.n	a10, 4
  4001c6:	0008e0        	callx8	a8
  4001c9:	0a8d      	mov.n	a8, a10
}
  4001cb:	082d      	mov.n	a2, a8
  4001cd:	f01d      	retw.n
	...

004001d0 <execve>:

int execve(const char *path, char *const argv[], char *const envp[])
{
  4001d0:	008136        	entry	a1, 64
  4001d3:	017d      	mov.n	a7, a1
  4001d5:	4729      	s32i.n	a2, a7, 16
  4001d7:	5739      	s32i.n	a3, a7, 20
  4001d9:	6749      	s32i.n	a4, a7, 24
   struct {const char *fname;  char * const *argv; char * const *env;} execcall = {.fname = path, .argv = argv, .env = envp};
  4001db:	4788      	l32i.n	a8, a7, 16
  4001dd:	0789      	s32i.n	a8, a7, 0
  4001df:	5788      	l32i.n	a8, a7, 20
  4001e1:	1789      	s32i.n	a8, a7, 4
  4001e3:	6788      	l32i.n	a8, a7, 24
  4001e5:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_EXEC, &execcall);
  4001e7:	ffa481        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  4001ea:	0888      	l32i.n	a8, a8, 0
  4001ec:	07bd      	mov.n	a11, a7
  4001ee:	05a0a2        	movi	a10, 5
  4001f1:	0008e0        	callx8	a8
  4001f4:	0a8d      	mov.n	a8, a10
}
  4001f6:	082d      	mov.n	a2, a8
  4001f8:	f01d      	retw.n
	...

004001fc <execv>:

int execv(const char *path, char *const argv[])
{
  4001fc:	006136        	entry	a1, 48
  4001ff:	017d      	mov.n	a7, a1
  400201:	0729      	s32i.n	a2, a7, 0
  400203:	1739      	s32i.n	a3, a7, 4
   return execve(path, argv, __environ);
  400205:	ff9b81        	l32r	a8, 400074 <_start-0x1c> (4012bc <__environ>)
  400208:	0888      	l32i.n	a8, a8, 0
  40020a:	08cd      	mov.n	a12, a8
  40020c:	17b8      	l32i.n	a11, a7, 4
  40020e:	0027a2        	l32i	a10, a7, 0
  400211:	fffbe5        	call8	4001d0 <execve>
  400214:	0a8d      	mov.n	a8, a10
}
  400216:	082d      	mov.n	a2, a8
  400218:	f01d      	retw.n
	...

0040021c <waitpid>:

int waitpid(int pid, int *status, int options)
{
  40021c:	008136        	entry	a1, 64
  40021f:	017d      	mov.n	a7, a1
  400221:	4729      	s32i.n	a2, a7, 16
  400223:	5739      	s32i.n	a3, a7, 20
  400225:	6749      	s32i.n	a4, a7, 24
   struct {int pid;  int *status; int options;} waitpidcall = {.pid = pid, .status = status, .options = options};
  400227:	4788      	l32i.n	a8, a7, 16
  400229:	0789      	s32i.n	a8, a7, 0
  40022b:	5788      	l32i.n	a8, a7, 20
  40022d:	1789      	s32i.n	a8, a7, 4
  40022f:	6788      	l32i.n	a8, a7, 24
  400231:	2789      	s32i.n	a8, a7, 8
   return do_syscall(SYSCALL_WAITPID, &waitpidcall);
  400233:	ff9181        	l32r	a8, 400078 <_start-0x18> (4012c0 <do_syscall>)
  400236:	0888      	l32i.n	a8, a8, 0
  400238:	07bd      	mov.n	a11, a7
  40023a:	06a0a2        	movi	a10, 6
  40023d:	0008e0        	callx8	a8
  400240:	0a8d      	mov.n	a8, a10
}
  400242:	082d      	mov.n	a2, a8
  400244:	f01d      	retw.n
	...

00400248 <event_handler>:

__attribute((weak)) void event_handler(int event, void *arg)
{
  400248:	006136        	entry	a1, 48
  40024b:	017d      	mov.n	a7, a1
  40024d:	0729      	s32i.n	a2, a7, 0
  40024f:	1739      	s32i.n	a3, a7, 4
   (void) event;
   (void) arg;
}
  400251:	f03d      	nop.n
  400253:	f01d      	retw.n
  400255:	000000        	ill

00400258 <main>:
#include "lib.h"

int main(int argc, char **argv)
{
  400258:	028136        	entry	a1, 0x140
  40025b:	017d      	mov.n	a7, a1
  40025d:	446722        	s32i	a2, a7, 0x110
  400260:	456732        	s32i	a3, a7, 0x114
   int readlen;
   char cmd[256];
   char *readlen_str = "readlen x\n";
  400263:	ff8781        	l32r	a8, 400080 <_start-0x10> (4002a8 <main+0x50>)
  400266:	006782        	s32i	a8, a7, 0
   while(1)
   {
      print("sh> ");
  400269:	ff8681        	l32r	a8, 400084 <_start-0xc> (4002b4 <main+0x5c>)
  40026c:	20a880        	or	a10, a8, a8
  40026f:	ff8681        	l32r	a8, 400088 <_start-0x8> (400190 <print>)
  400272:	0008e0        	callx8	a8
      readlen = read(0, cmd, 256);
  400275:	878b      	addi.n	a8, a7, 8
  400277:	00a1c2        	movi	a12, 0x100
  40027a:	08bd      	mov.n	a11, a8
  40027c:	0a0c      	movi.n	a10, 0
  40027e:	ff8381        	l32r	a8, 40008c <_start-0x4> (4000ec <read>)
  400281:	0008e0        	callx8	a8
  400284:	17a9      	s32i.n	a10, a7, 4
      if(readlen < 1)
  400286:	1788      	l32i.n	a8, a7, 4
  400288:	dd18e6        	bgei	a8, 1, 400269 <main+0x11>
      {
         readlen_str[8] = '0' + readlen;
  40028b:	1788      	l32i.n	a8, a7, 4
  40028d:	749080        	extui	a9, a8, 0, 8
  400290:	0788      	l32i.n	a8, a7, 0
  400292:	888b      	addi.n	a8, a8, 8
  400294:	30c992        	addi	a9, a9, 48
  400297:	749090        	extui	a9, a9, 0, 8
  40029a:	004892        	s8i	a9, a8, 0
         print(readlen_str);
  40029d:	07a8      	l32i.n	a10, a7, 0
  40029f:	ff7a81        	l32r	a8, 400088 <_start-0x8> (400190 <print>)
  4002a2:	0008e0        	callx8	a8
      print("sh> ");
  4002a5:	fff006        	j	400269 <main+0x11>
