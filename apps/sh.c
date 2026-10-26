#include "lib.h"

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
   //else implement environment variables and get home
}

int echo_handler(char **args)
{
   args++;
   while(*args)
   {
      print(*args);
      print(" ");
      args++;
   }
   print("\n");
}

sh_builtin builtins[] = 
{
   "cd", cd_handler,
   "echo", echo_handler,
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
   print(buff);
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

void sh_program_runner(char **args)
{
   execv(args[0],args);
   print("failed to run program ");
   print(args[0]);
   print("\n");
}

int run_program(char **cmd)
{
   int pid = thread_create(sh_program_runner,cmd);
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
      print("sh> ");
      while(1)
      {
         readlen = read(0, &inchar, 1);
         if(readlen < 0)
         {
            print("read error\n");
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
