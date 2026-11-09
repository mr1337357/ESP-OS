#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "libc.h"

typedef struct
{
   char *name;
   int (*handler)(char **);
} sh_builtin;

int cd_handler(char **args)
{
   if(args[1])
   {
      chdir(args[1]);
   }
   else
   {
      char *home = getenv("HOME");
      if(home)
      {
         chdir(home);
      }
   }
}

extern char **__environ;
int env_handler(char **args)
{
   int i;
   char *var;
   args++;
   if(!__environ)
   {
      return 0;
   }
   if(args[0])
   {
      var = getenv(args[0]);
      if(var)
      {
         printf(var);
         printf("\n");
      }
   }
   else
   {
      for(i=0;__environ[i];i++)
      {
         printf(__environ[i]);
         printf("\n");
      }
   }
   return 0;
}


sh_builtin builtins[] = 
{
   "cd", cd_handler,
   "env", env_handler,
};

#define NUM_BUILTINS (sizeof(builtins)/sizeof(builtins[0]))

void hexdump(char c)
{
   char buff[3];
   char *hex = "0123456789ABCDEF";
   unsigned char uc = (unsigned char)c;
   buff[0] = hex[(uc>>4)];
   buff[1] = hex[(uc&0x0F)];
   buff[2] = 0;
   printf(buff);
}

char *strtoke(char *input, char *delim)
{
   static char *mem;
   char *temp;
   if(input)
   {
      mem = input;
   }
   if(mem == 0)
   {
      return 0;
   }
   temp = mem;
   //we're going to ignore delim cause I'm lazy
   while(*temp == ' ')
   {
      temp++;
   }
   if(*temp == 0)
   {
      return 0;
   }
   mem = temp;
   while(*mem != ' ')
   {
      if(*mem == 0)
      {
         mem = 0;
         return temp;
      }
      mem++;
   }
   *mem = 0;
   mem++;
   return temp;
}

int handle_builtins(char **cmd)
{
   int i;
   for(i=0;i<NUM_BUILTINS;i++)
   {
      if(!strcmp(cmd[0],builtins[i].name))
      {
         builtins[i].handler(cmd);
         return 1;
      }
   }
   return 0;
}

char *redir_out;
char *redir_in;

void sh_program_runner(char **args)
{
   int outfd;
   int infd;
   if(redir_out)
   {
      outfd = open(redir_out,O_WRONLY|O_CREAT);
      if(outfd < 0)
      {
         printf("couldn't open outfd\n");
         return;
      }
      dup2(outfd,1);
   }
   if(redir_in)
   {
      infd = open(redir_in,O_RDONLY|O_CREAT);
      if(infd < 0)
      {
         printf("couldn't open infd\n");
         return;
      }
      dup2(infd,0);
   }
   execvp(args[0],args);
   printf("failed to run program ");
   printf(args[0]);
   printf("\n");
}

int run_program(char **cmd)
{
   int i;
   int pid;
   redir_out = 0;
   redir_in = 0;
   for(i=0;cmd[i];i++)
   {
      if(!strcmp(cmd[i],">"))
      {
         cmd[i] = 0;
         i++;
         if(!cmd[i])
         {
            return -1;
         }
         redir_out = cmd[i];
      }
      if(!strcmp(cmd[i],"<"))
      {
         cmd[i] = 0;
         i++;
         if(!cmd[i])
         {
            return -1;
         }
         redir_in = cmd[i];
      }
   }
   pid = thread_create(sh_program_runner,cmd);
   waitpid(pid,0,0);
   return 0;
}

void handle_command(char *cmd)
{
   char *temp;
   int numtokens = 0;
   char *tokens[10];
   int i;
   for(temp = strtoke(cmd," "); temp; temp = strtoke(0," "))
   {
      if(numtokens >= 9)
      {
         break;
      }
      tokens[numtokens++] = temp;
   }
   tokens[numtokens] = 0;
   if(!handle_builtins(tokens))
   {
      run_program(tokens);
   }
}

int main(int argc, char **argv)
{
   int cmdoffset = 0;
   int readlen;
   char cmd[256];
   char inchar;
   while(1)
   {
      cmdoffset = 0;
      printf("sh> ");
      while(1)
      {
         readlen = read(0, &inchar, 1);
         if(readlen < 0)
         {
            printf("read error\n");
            return 1;
         }
         else
         {
            //hexdump(inchar);
            if(inchar == '\n')
            {
               cmd[cmdoffset] = 0;
               handle_command(cmd);
               break;
            }
            else
            {
               if(cmdoffset < 80)
               {
                  cmd[cmdoffset] = inchar;
                  cmdoffset++;
               }
               else
               {
                  write(1,"\b",1);
               }

            }
         }
      }
   }
   return 0;
}
