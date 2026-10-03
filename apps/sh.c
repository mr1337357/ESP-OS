#include "lib.h"

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

void handle_command(char *cmd)
{

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
